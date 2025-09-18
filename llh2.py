from __future__ import annotations
from llparse import LLParse
from llparse.pybuilder.main_code import Node, _Match as CMatch
from enum import IntEnum, auto, IntFlag
from typing import TypedDict
from pathlib import Path



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
    # The only 2 that have a chance to overflow
    LENGTH_OVERFLOW = auto()
    STREAM_ID_OVERFLOW = auto()

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

    # User Chose to Exit from the loop after parsing a frame
    EXIT = auto()
    USER = auto()


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

    res += "#ifdef __cplusplus\n"
    res += '}  /* extern "C" */\n'
    res += "#endif\n"
    res += "#endif  /* LLH2_C_HEADERS__H__ */\n"

    return res


class FrameParser:
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
            # Frame Payload if any should exist or not...
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
            {k.value: v for k, v in Map.items()},
        )

        if Next is not None:
            res.otherwise(Next)
        return res

    def build(self):
        p = self.llparse
        p.property("ptr", "settings")
        p.property("i32", "length")
        p.property("i32", "stream_id")
        p.property("i8", "type")
        p.property("i8", "flags")

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

        # stream_id
        stream_id.skipTo(body_start)

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
    p = FrameParser(llh2)
    src = llh2.build(
        p.build(), header_name="llh2", headerGuard="INCLUDE_LLH2_ITSELF_H_"
    )
    build_folder = Path("build")
    (build_folder / "llh2.c").write_text(src.c)
    (build_folder / "llh2.h").write_text(buildCHeaders() + src.header)
