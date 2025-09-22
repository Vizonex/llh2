from __future__ import annotations
from abc import ABC, abstractmethod
from dataclasses import dataclass

from llparse import LLParse
from llparse.pybuilder.main_code import _Match as CodeMatch, Span, Node

from enum import IntEnum, IntFlag
from typing import Literal
from .error import Error
from llparse.constants import UNSIGNED_TYPES

# Because llh2 is slightly bigger than llhttp
# we needed to change our apporch to make
# everything as easy as possible
# so going object oridented was a must


def cleanup_node_dict(ndict: dict[IntEnum | IntFlag | int, Node]) -> dict[int, Node]:
    """Cleans up enums from the dictionary and transforms them to integers"""
    transform = lambda x: x.value if isinstance(x, (IntEnum, IntFlag)) else int(x)
    return {transform(k): v for k, v in ndict.items()}


class LLExt(ABC):
    """Abstract extension related to LLParse"""

    def __init__(self, llparse: LLParse):
        self.llparse = llparse
    
    @abstractmethod
    def do_build(self) -> str:
        """Generates custom code that llparse may not be able to generate"""

    def pause(self, msg: str, next: Node | None = None):
        res = self.llparse.pause(Error.PAUSED.value, msg)
        if next:
            res.otherwise(next)
        return res

    def update(self, field: str, value: int, next: Node | None = None):
        p = self.llparse
        res = p.invoke(p.code.update(field, value))
        if next:
            res.otherwise(next)
        return res

    def is_equal(self, field: str, value: int, equal: Node, not_equal: Node):
        # bug with isEqual accepting value as str will fix later...
        p = self.llparse
        return p.invoke(p.code.isEqual(field, value), {0: not_equal}, equal)  # type: ignore

    def load(
        self,
        field: str,
        record: dict[IntEnum | IntFlag | int, Node],
        next: Node | None = None,
    ):
        p = self.llparse
        res = p.invoke(p.code.load(field), cleanup_node_dict(record))
        if next:
            res.otherwise(next)
        return res

    def test(self, field: str, value: IntEnum | IntFlag | int, yes: Node, no: Node):
        """Test if a specific flag is set or not"""
        p = self.llparse
        return p.invoke(
            p.code.test(field, value.value if not isinstance(value, int) else value),
            {0: no},
            yes,
        )
    def is_le(self, field:str, value: int, le: Node, not_le: Node):
        p = self.llparse
        return p.invoke(p.code.is_le(field, value), {0: not_equal}, equal)  # type: ignore


    def error(self, value: Error | IntEnum | int, reason:str):
        if isinstance(value, IntEnum):
            value = value.value
        return self.llparse.error(value, reason)

    


class CodeWrap(LLExt):
    """Defines a C Wrapper related function for `llparse.code.match`
    these sections of code have to be automated on llh2's end instead
    of llparse's end to ease several burdens"""

    def __init__(self, llparse: LLParse, name: str):
        super().__init__(llparse)
        self.full_name = f"{llparse.prefix}__{name}"
        self.match = llparse.code.match(self.full_name)
        self.name = name

    def invoke(
        self,
        ndict: dict[IntEnum | IntFlag | int, Node] | Node | None = None,
        otherwise: Node | None = None,
    ):
        return self.llparse.invoke(self.match, cleanup_node_dict(ndict), otherwise)


class MatchWrap(CodeWrap):
    """Related to the C Wrapper for `llh2_cb` in `llh2_settings_t`"""

    def error(self, error_code: Error):
        return self.llparse.error(error_code.value, f"{self.name!r} callback error")

    def invoke_pausable(self, error_code: Error, next: Node) -> Node:
        return self.invoke(
            {0: next, Error.PAUSED: self.pause(f"{self.name} pause", next)},
            self.error(error_code),
        )

    

    def do_build(self) -> str:
        code = f"int {self.full_name} (\n"
        code += "    llh2_t *s, const unsigned char *p,\n"
        code += "    const unsigned char *endp){\n"
        code += "    int err;\n"
        code += f"    H2_CALLBACK_MAYBE(s, {self.name});\n"
        code += "    return err;\n};\n"
        return code

    @property
    def cb_property(self) -> str:
        """Writes callback property"""
        return f"  llh2_cb {self.name};"


class SpanWrap(LLExt):
    """Span wrapper for C Wrapper to wrap in a match callback and one span"""

    def __init__(self, llparse: LLParse, name: str) -> None:
        super().__init__(llparse)
        self.full_name = f"{llparse.prefix}__{name}"
        self.span = llparse.span(llparse.code.span(self.full_name))
        self.name = name
        self.complete_cb = MatchWrap(llparse, f"{name}_complete")

    def start(self, otherwise: Node | None):
        return self.span.start(otherwise)

    def end(self, error_code: Error, next: Node):
        return self.span.end(self.complete_cb.invoke_pausable(error_code, next))

    def do_build(self) -> str:
        code = f"/* === Span Callback {self.name} === */\n\n"
        # code += self.complete_cb.do_build() + "\n"

        code += f"int {self.full_name} (\n"
        code += "    llh2_t *s, const unsigned char *p,\n"
        code += "    const unsigned char *endp){\n"
        code += "    int err;\n"
        code += f"    H2_SPAN_CALLBACK_MAYBE(s, {self.name}, p, endp - p);\n"
        code += "    return err;\n};"
        return code

    @property
    def cb_property(self) -> str:
        """Writes callback property"""
        return f"  llh2_data_cb {self.name};"


BITDICT = {"i8": 1, "i16": 2, "i32": 4}


class IntField(LLExt):
    """Writes a consumable integer base property as part of phase 1 of the parser
    phase 2 requires unions due to the extreme amount of different attributes frames
    can be possibly carrying"""

    def __factory__(self, field:str, ty: Literal['i8', 'i16', 'i32'], bits:int | None):
        return self.llparse.intLE(field, bits or BITDICT[ty])

    def __init__(
        self,
        llparse: LLParse,
        field: str,
        ty: Literal["i8", "i16", "i32"],
        error_code: Error,
        bits: int | None = None,
    ):
        super().__init__(llparse)
        
        if all([field != p.name for p in llparse.properties()]):
            # Create new field if property is non-exitstant so
            # repeated setups can be done safely with other shared nodes
            llparse.property(ty, field)
        self.node = self.__factory__(field, ty, bits)
        self.err = error_code
        self.on_complete = MatchWrap(llparse, f"on_{field}_complete")
        self.field = field

    def consume(self, next: Node):
        return self.node.skipTo(self.on_complete.invoke_pausable(self.err, next))

    def consume_and_complete_after(self, send: Node, next:Node):
        """Handle something before sending through to be invoked"""
        self.node.skipTo(send)
        return send.otherwise(self.on_complete.invoke_pausable(self.err, next))

    @property
    def cb_property(self) -> str:
        return self.on_complete.cb_property

    def do_build(self) -> str:
        return self.on_complete.do_build()

class UIntField(IntField):
    def __factory__(self, field:str, ty: Literal['i8', 'i16', 'i32'], bits:int | None):
        return self.llparse.uintLE(field, bits or BITDICT[ty])

UNSIGNED_TYPES: dict[str, str] = {
    "i8": "uint8_t",
    "i16": "uint16_t",
    "i32": "uint32_t",
    "i64": "uint64_t",
}


# class UnionField(LLExt):
#     def __init__(self, llparse: LLParse, shared_field:str, union:str, field:str, ty:Literal['i8', 'i16', 'i32'], bits: int | None = None):
#         super().__init__(llparse)
#         self.union = union
#         self.field = field
#         self.ty = ty
#         self.name = llparse.prefix + f'__share_{shared_field}'
#         self.share_cb = llparse.code.match(self.name)
#         self.node = llparse.intLE(shared_field, bits or BITDICT[ty])
#         self.shared_field = shared_field


#     def as_field(self) -> str:
#         return f'  {UNSIGNED_TYPES[self.ty]} {self.field};'

#     def consume(self, next:Node):
#         """hands off shared_field given from the parser to the union filed

#         psudo code.
#         ```c
#         state->frame_union->frame_type->frame_field = state->_dummy_int;
#         /* reset after use... */
#         state->_dummy_int = 0;
#         return 0;
#         ```
#         """
#         p = self.llparse
#         return self.node.skipTo(p.invoke(self.share_cb).otherwise(next))


#     def do_build(self) -> str:
#         code = f"int {self.name}(\n"
#         code += "    llframe_t *s, const unsigned char *p,\n"
#         code += "    const unsigned char *endp){\n"
#         code += f"   s->_u->{self.union}->{self.field} = s->{self.shared_field};\n"
#         code += f'    s->{self.shared_field} = 0;'
#         code += '    return 0;\n};\n'
#         return code


# class UnionStruct(LLExt):
#     """Used for defining different frames for calling back to when completed"""

#     def __init__(self, llparse: LLParse, parent:str, name:str , shared:str) -> None:
#         super().__init__(llparse)
#         self.type_name = f'llh2_{name}'
#         self.name = name
#         self.parent = parent
#         self.shared = shared
#         self.fields: list[UnionField] = []

#     def as_field(self) -> str:
#         return f'  {self.type_name}_t {self.name};'

#     def field(self, name:str, ty: Literal['i8', 'i16', 'i32'], bits: int | None = None):
#         f = UnionField(self.llparse, self.shared, self.name, name, ty, bits)
#         self.fields.append(f)
#         return f

#     def do_build(self) -> str:
#         code = f'typedef struct {self.type_name}_s {self.type_name}_t;\n\n'
#         code += f"struct {self.type_name}_s" + ' {\n'
#         for field in self.fields:
#             code += f'{field.as_field()}\n'
#         code += '};\n'
#         return code


# class UnionParent(LLExt):
#     def __init__(self, llparse: LLParse, name:str, shared_property:str):
#         super().__init__(llparse)
#         self.name = name
#         self.type = f'llh2_{name}'
#         self.shared = shared_property
#         self.structures: list[UnionStruct] = []

#     def struct(self, name:str):
#         us = UnionStruct(self.llparse, self.name, name, self.shared)
#         self.structures.append(us)
#         return us

#     def do_build(self):
#         code = f'typedef union {self.type}_u {self.type}_t;\n\n'
#         for field in self.structures:
#             code += field.do_build() + '\n'

#         code += f'union {self.type}_u' + ' {\n'
#         for field in self.structures:
#             code += f'{field.as_field()}\n'
#         code += '};\n'
#         return code


# class LLFrame:
#     def __init__(self, llparse: LLParse) -> None:
#         self.llparse = llparse
#         # dummy property to pass off
#         self.llparse.property('i32', '_dummy')
#         self.frame_union = UnionParent(self.llparse, 'frame_union', '_dummy')
