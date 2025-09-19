from __future__ import annotations
from llparse import LLParse
from llparse.pybuilder.main_code import Node, _Match as CMatch
from enum import IntEnum, auto, IntFlag
from typing import TypedDict, NamedTuple
from pathlib import Path
import struct
from abc import abstractmethod, ABC


class Encoding(IntEnum):
    NONE = 0
    HEX = 1


def buildEnum(name: str, prefix: str, e: IntEnum, encoding: Encoding = Encoding.NONE):
    res = ""
    _name = name.lower()
    for k, v in e._member_map_.items():
        if res:
            res += ",\n"
        res += f"  {prefix}_{k} = "
        if encoding == Encoding.HEX:
            res += hex(v.value)
        else:
            res += f"{v.value}"

    return (
        f"enum {_name} " + "{\n" + res + "\n};\n" + f"typedef enum {_name} {_name}_t;\n"
    )


def buildMap(name: str, d: dict[str, int]):
    name = name.upper()
    res = f"#define {name}_MAP(XX) \\\n"
    for k, v in sorted(d.items()):
        res += f"  XX({v}, {k.replace('-', '')}, {k}) \\\n"
    return res + "\n"


class UnpackCB(TypedDict):
    length: CMatch
    stream_id: CMatch
    type: CMatch
    flag: CMatch


class Error(IntEnum):
    OK = 0
    PAUSED = 1

    # CALLBACKS
    CB_ON_FRAME_START = auto()
    CB_ON_FRAME_END = auto()
    CB_ON_LENGTH = auto()
    CB_ON_STREAM_ID = auto()
    CB_ON_TYPE = auto()
    CB_ON_FLAG = auto()
    CB_ON_BODY_START = auto()
    CB_ON_BODY_END = auto()
    CB_ON_RESET = auto()

    CB_ON_SETTINGS_IDENTIFIER = auto()
    CB_ON_SETTINGS_VALUE = auto()

    CB_ON_PROMISE_STREAM_ID = auto()

    # Errors
    INVALID_SETTINGS_IDENTIFIER = auto()
    INVALID_LENGTH = auto()
    INVALID_PAD_LENGTH = auto()

    # User Chose to Exit from the loop after parsing a frame
    EXIT = auto()
    USER = auto()


class FrameType(IntEnum):
    """Different possible frame types for the parse to go through
    and identify"""

    DATA = 0x00
    HEADERS = 0x01
    PRIORITY = 0x02
    RST_STREAM = 0x03
    SETTINGS = 0x04
    PUSH_PROMISE = 0x05
    PING = 0x06
    GOAWAY = 0x07
    WINDOW_UPDATE = 0x08
    CONTINUATION = 0x09


class AckFlags(IntFlag):
    NONE = 0x00
    ACK = 0x01


class StreamFlags(IntFlag):
    NONE = 0x00  # 0
    END_STREAM = 0x01  # 1
    END_HEADERS = 0x04  # 4
    PADDED = 0x08  # 8
    PRIORITY = 0x20  # 32


class SettingsIndentifiers(IntEnum):
    # Documentation comes from hyperframe https://pypi.org/projects/hyperframe

    #: The byte that signals the SETTINGS_HEADER_TABLE_SIZE setting.
    HEADER_TABLE_SIZE = 0x01
    #: The byte that signals the SETTINGS_ENABLE_PUSH setting.
    ENABLE_PUSH = 0x02
    #: The byte that signals the SETTINGS_MAX_CONCURRENT_STREAMS setting.
    MAX_CONCURRENT_STREAMS = 0x03
    #: The byte that signals the SETTINGS_INITIAL_WINDOW_SIZE setting.
    INITIAL_WINDOW_SIZE = 0x04
    #: The byte that signals the SETTINGS_MAX_FRAME_SIZE setting.
    MAX_FRAME_SIZE = 0x05
    #: The byte that signals the SETTINGS_MAX_HEADER_LIST_SIZE setting.
    MAX_HEADER_LIST_SIZE = 0x06
    #: The byte that signals SETTINGS_ENABLE_CONNECT_PROTOCOL setting.
    ENABLE_CONNECT_PROTOCOL = 0x08


class H2Parser_Settings(IntFlag):
    NONE = 0
    STRICT = auto()  # Enables checking frames for common settings or unknown settings


class Reset(IntEnum):
    ERROR = -1
    PAUSE = 0
    EXIT = 1


def buildCHeaders():
    res = ""
    res += "#ifndef LLH2_C_HEADERS__H__\n"
    res += "#define LLH2_C_HEADERS__H__\n"

    res += "#ifdef __cplusplus\n"
    res += 'extern "C" {\n'
    res += "#endif\n"

    res += "\n"

    res += buildEnum("llh2_reset_flag", "H2RF", Reset) + "\n"
    res += buildEnum("llh2_errno", "H2PE", Error) + "\n"

    res += buildEnum("llh2_misc_flags", "H2MF", AckFlags) + "\n"
    res += buildEnum("llh2_stream_flags", "H2SF", StreamFlags) + "\n"

    res += buildEnum("llh2_frame_type", "H2FT", FrameType, Encoding.HEX) + "\n"

    res += buildMap("LLH2_FRAME_TYPE_MAP", FrameType._member_map_) + "\n"

    res += "#ifdef __cplusplus\n"
    res += '}  /* extern "C" */\n'
    res += "#endif\n"
    res += "#endif  /* LLH2_C_HEADERS__H__ */\n"

    return res





class BaseParser(ABC):
    """Base plate for building parsers to make things a bit easier to setup"""

    def __init__(self, llparse: LLParse):
        self.llparse = llparse
        self.u_cb: dict[str, CMatch] = {}

    def pause(self, msg: str, Next: Node | None = None):
        res = self.llparse.pause(Error.PAUSED.value, msg)
        if Next != None:
            res.otherwise(Next)
        return res

    def invoke_pausable(self, name: str, Next: Node, error_code: Error):
        """Used for calling settings callbacks that can be interacted with by the end developer/user"""
        p = self.llparse
        return p.invoke(
            self.u_cb[name],
            {0: Next, int(Error.PAUSED): self.pause(f"{name} pause", Next)},
            p.error(error_code.value, f"{name!r} callback error"),
        )

    def invoke_internal(
        self, name: str, Next: Node, error_code: Error, error_reason: str
    ):
        """inner related matches that should be used in http2.c"""
        p = self.llparse
        return p.invoke(
            self.u_cb[name],
            {0: Next},
            p.error(error_code.value, error_reason),
        )

    def testFlag(
        self,
        flag: H2Parser_Settings,
        yes: Node,
        no: Node | None = None,
    ) -> Node:
        p = self.llparse

        res = p.invoke(
            p.code.test("h2_flag", flag.value),
            {1: yes},
        )
        if no is not None:
            res.otherwise(no)
        return res

    def load(
        self, field: str, options: dict[int | str, Node], otherwise: Node | None = None
    ):
        p = self.llparse
        invoke = p.invoke(p.code.load(field), options)
        if otherwise:
            invoke.otherwise(otherwise)
        return invoke

    @abstractmethod
    def build(self, end: Node) -> Node:
        """Builds a frame parser to work with
        :param end: the node to goto when finished"""


class SettingsFrame(BaseParser):
    """
    Figure 7 of RFC 9113
    SETTINGS Frame {
        Length (24),
        Type (8) = 0x04,
        Unused Flags (7),
        ACK Flag (1),
        Reserved (1),
        Stream Identifier (31) = 0,
        Setting (48) ...,
    }

    Setting {
        Identifier (16),
        Value (32),
    }

    """

    def __init__(self, llparse: LLParse):
        super().__init__(llparse)
        p = llparse
        # Strictness will come in a future update
        self.u_cb = {
            "on_setting_identifier": p.code.match("llh2__on_setting_identifier"),
            "on_setting_value": p.code.match("llh2__on_setting_value"),
        }

    def settings(self, size: int, Next: Node):
        p = self.llparse
        value = p.uintLE("setting_value", size).skipTo(
            self.invoke_pausable("on_setting_value", Next, Error.CB_ON_SETTINGS_VALUE)
        )
        return self.settings_prelude(value)

    # end is related to callback on_frame_end
    def build(self, end: Node) -> Node:
        p = self.llparse
        p.property("i16", "setting_ident")
        p.property("i32", "setting_value")
        subtract_cb = p.code.match('llh2__settings_subtract')

        ident = p.node('settings_ident')
        # if ACK do nothing...
        start = self.load('flags', {1: end}, ident)
 
        invalid_ident = p.error(Error.INVALID_SETTINGS_IDENTIFIER.value, 'settings identifier is invalid')

        # Better strict then sorry...   
        # increments by 6 if less than or equal to zero, then stop
        selection: dict[str, int] = {'\x00' + chr(v.value): v.value for v in SettingsIndentifiers._member_map_.values()}
        
        value = p.uintLE('setting_value', 4)

        # invalid if it doesn't match '\x00' first since numbers are below 256
        
        ident.select(selection, p.invoke(p.code.store('setting_ident'), self.invoke_pausable('on_setting_identifier', value, Error.CB_ON_SETTINGS_IDENTIFIER))).otherwise(invalid_ident)        

        sub = p.invoke(subtract_cb, {0: end}).otherwise(ident)

        value.skipTo(
            self.invoke_pausable('on_setting_value', sub , Error.CB_ON_SETTINGS_VALUE)
        )

        return start


class PushPromiseFrame(BaseParser):
    def __init__(self, llparse):
        super().__init__(llparse)
        self.u_cb['on_promise_stream_id'] = llparse.code.match('llh2__on_promise_stream_id')

    def testFrameFlag(
        self,
        flag: int | IntEnum,
        Map: dict[IntEnum | IntFlag, Node],
        Next: Node | None = None,
    ) -> Node:
        p = self.llparse

        res = p.invoke(
            p.code.test("flags", int(flag)),
            # Fix enums here because were all lazy
            Map
        )

        if Next is not None:
            res.otherwise(Next)
        return res


    def build(self, end:Node):
        p = self.llparse
        p.property('i8', 'pad_length')
        p.property('i32', 'promise_stream_id')
        
        pad_length = p.uintLE('pad_length', 1)
        promise_stream_id = p.uintLE('promise_stream_id', 4)

        start = self.testFrameFlag(
            StreamFlags.PADDED, {1: pad_length}, promise_stream_id
        )
        pad_length.skipTo(promise_stream_id)
        
        # end should be tied to on_body span
        sub = p.invoke(p.code.match('llh2__subtract_push_promise_length'), {0:end},  p.error(Error.INVALID_PAD_LENGTH.value, "invalid pad length provided"))
        promise_stream_id.skipTo(self.invoke_pausable('on_promise_stream_id', sub, Error.CB_ON_PROMISE_STREAM_ID))
 
        return start


class H2Parser:
    """Main Parser"""

    def __init__(self, llparse: LLParse):
        self.llparse = llparse
        p = llparse

        self.u_cb = {
            "on_frame_start": p.code.match("llh2__on_frame_start"),
            "on_frame_end": p.code.match("llh2__on_frame_end"),
            "on_length": p.code.match("llh2__on_length"),
            # Part of stream_id I might look into fixing the reserved bit...
            "on_stream_id": p.code.match("llh2__on_stream_id"),
            "on_type": p.code.match("llh2__on_type"),
            "on_flags": p.code.match("llh2__on_flags"),
            # Frame Payload if any should exist or not in relation to a data or header / continuation frame...
            "on_body_start": p.code.match("llh2__on_body_start"),
            "on_body_end": p.code.match("llh2__on_body_end"),
            # Frame Resetting...
            # 0 -> Pause
            # 1 -> Exit
            # -1 -> Error
            "on_reset": p.code.match("llh2__on_reset"),
        }

        self.on_reset = p.node("on_reset")
        self.exit = p.node("exit")

        self.on_body = p.span(p.code.span("llh2__on_body"))

    def pause(self, msg: str, Next: Node | None = None):
        res = self.llparse.pause(Error.PAUSED.value, msg)
        if Next != None:
            res.otherwise(Next)
        return res

    def invoke_pausable(self, name: str, Next: Node, error_code: Error):
        p = self.llparse
        return p.invoke(
            self.u_cb[name],
            {0: Next, int(Error.PAUSED): self.pause(f"{name} pause", Next)},
            p.error(error_code.value, f"{name!r} callback error"),
        )

    def testFlag(
        self,
        flag: int | IntEnum,
        Map: dict[IntEnum | IntFlag, Node],
        Next: Node | None = None,
    ) -> Node:
        p = self.llparse

        res = p.invoke(
            p.code.test("flags", int(flag)),
            # Fix enums here because were all lazy
            Map,
        )

        if Next is not None:
            res.otherwise(Next)
        return res

    def build(self):
        p = self.llparse
        p.property("ptr", "settings")
        p.property("i32", "length")
        p.property("i32", "_sub_length") # length used for subtraction (internal)

        p.property("i32", "stream_id")
        p.property("i8", "type")
        p.property("i8", "flags")

        # h2_state is related to idle or other flags like headers already sent etc...
        p.property("i8", "h2_state")
        p.property("i8", "h2_flag")  # parser settings such as strict or not

        p.property("i8", "weight")
        

        length = p.uintLE("length", bits=3)
        ty = p.uintLE("type", bits=1)
        flags = p.uintLE("flags", bits=1)
        stream_id = p.uintLE("stream_id", bits=4)

        ty.skipTo(self.invoke_pausable("on_type", flags, Error.CB_ON_TYPE))
        flags.skipTo(self.invoke_pausable("on_flags", stream_id, Error.CB_ON_FLAG))
        length.skipTo(self.invoke_pausable("on_length", ty, Error.CB_ON_LENGTH))

        start = self.invoke_pausable("on_frame_start", length, Error.CB_ON_FRAME_START)

        end = self.invoke_pausable("on_frame_end", self.on_reset, Error.CB_ON_FRAME_END)

        
        

        body_end = self.invoke_pausable("on_body_end", end, Error.CB_ON_BODY_END)

        body_start = self.invoke_pausable(
            "on_body_start",
            self.on_body.start(
                p.consume("length").otherwise(self.on_body.end(body_end))
            ),
            Error.CB_ON_BODY_START,
        )

        check_frame_types = p.invoke(p.code.load('type'), 
            {
                0x04 : SettingsFrame(p).build(end),
                0x05: PushPromiseFrame(p).build(body_start)
            }, body_start)
        
        # stream_id
        stream_id.skipTo(check_frame_types)

        self.on_reset.otherwise(
            p.invoke(
                self.u_cb["on_reset"],
                {
                    Reset.PAUSE.value: self.pause("paused parser at on_reset", start),
                    Reset.EXIT.value: self.exit,
                },
            ).otherwise(p.error(Error.CB_ON_RESET, "on_reset callback error"))
        )
        self.exit.skipTo(self.exit)
        return start


if __name__ == "__main__":
    llh2 = LLParse("llh2__internal")
    p = H2Parser(llh2)
    src = llh2.build(
        p.build(), header_name="llh2", headerGuard="INCLUDE_LLH2_ITSELF_H_"
    )
    build_folder = Path("build")
    (build_folder / "llh2.c").write_text(src.c)
    (build_folder / "llh2.h").write_text(buildCHeaders() + src.header)
