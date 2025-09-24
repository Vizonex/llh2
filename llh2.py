from __future__ import annotations
from llparse import LLParse
from llparse.pybuilder.main_code import Node, _Match as CMatch
from enum import IntEnum, auto, IntFlag
from typing import TypedDict, NamedTuple, TypeVar, Callable, TypeAlias, Literal
from pathlib import Path
import struct
from abc import abstractmethod, ABC
from llh2_tools.error import Error
from llh2_tools.abstract import LLExt, SpanWrap, IntField, MatchWrap, UIntField


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
    ALTSVC = 0x0A



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


CHEADERS_CWRAPPER_PROLOUGE = """

#ifndef __LLH2_CWRAPPER_SECTION
#define __LLH2_CWRAPPER_SECTION

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__wasm__)
#define LLH2_EXPORT __attribute__((visibility("default")))
#elif defined(_WIN32)
#define LLH2_EXPORT __declspec(dllexport)
#else
#define LLH2_EXPORT
#endif

typedef llh2__internal_t llh2_t;
typedef struct llh2_settings_s llh2_settings_t;

typedef int (*llh2_data_cb)(llh2_t*, const char *at, size_t length);
typedef int (*llh2_cb)(llh2_t*);

struct llh2_settings_s {"""

CHEADERS_CWRAPPER_EPILOUGE = """
/// @brief Initalizes a new http/2 parser
/// @param parser the parser to initalize
/// @param settings the callbacks to give to this parser
LLH2_EXPORT
void llh2_init(llh2_t* parser, const llh2_settings_t* settings);

/// @brief resets parser excluding the setting callbacks if any were given
/// @param parser the parser to be reset
LLH2_EXPORT
void llh2_reset(llh2_t* parser);

/// @brief obtains the type from the given frame if any are provided
/// @param parser the parser to obtain the frame's type from
/// @return the type of frame 
LLH2_EXPORT 
uint8_t llh2_get_type(llh2_t* parser);

/// @brief Obtains the frame's flags from the given parser
/// @param parser the parser to obtain the flags from
/// @return the parsed frame's flags
LLH2_EXPORT
uint8_t llh2_get_flags(llh2_t* parser);

/// @brief Obtains the frame's body/payload size
/// @param parser the parser to obtain the length of the body
/// @return the parsed frame's payload size
LLH2_EXPORT
uint32_t llh2_get_length(llh2_t* parser);

/// @brief Obtains the stream_id for the given frame
/// @param parser the parser to obtain the stream_id from
/// @return the parsed frame's stream_id
LLH2_EXPORT
uint32_t llh2_get_stream_id(llh2_t* parser);

/// @brief Resumes parser if it has been paused.
/// @param parser the parser to unpase and resume parsing data with.
LLH2_EXPORT
void llh2_resume(llh2_t* parser);

/// @brief Execute the parser and give datastream of frames to parse
/// @param parser the parser to execute data from
/// @param data the data that will be parsed
/// @param len the length of the data stream
/// @return errono if raised otherwise this will result in H2PE_OK which is 0
LLH2_EXPORT 
llh2_errno_t llh2_execute(llh2_t* parser, const char* data, size_t len);

/// @brief Get String Name of the Provided type
/// @return "UNKNOWN" if type is unknown otherwise return the Frame's Name
LLH2_EXPORT
const char* llh2_get_type_name(llh2_t* parser);

LLH2_EXPORT
void llh2_settings_init(llh2_settings_t* settings);

#ifdef __cplusplus
}
#endif

#endif // __LLH2_CWRAPPER_SECTION
"""


CAPI_PROLOUGE = """

#include "llh2.h"
#include <stdio.h>

/* Unlike llhttp a decision early on was made to Autogenerate 
 * this section of code due to it's increase in size comapred to llhttp 
 * due to having more callbacks to write in or change overtime as 
 * new things get introdued to the http/2 protocol as it's still a 
 * bit newer and not completely mature yet.
 */

// Don't want to override llhttp's work now do we?
#define H2_CALLBACK_MAYBE(PARSER, NAME)                         \\
    do {                                                        \\
        const llh2_settings_t *settings;                        \\
        settings = (const llh2_settings_t *)(PARSER)->settings; \\
        if (settings == NULL || settings->NAME == NULL)         \\
        {                                                       \\
            err = 0;                                            \\
            break;                                              \\
        }                                                       \\
        err = settings->NAME((PARSER));                         \\
    } while (0)

#define H2_SPAN_CALLBACK_MAYBE(PARSER, NAME, START, LEN)                      \\
    do {                                                                      \\
        const llh2_settings_t *settings;                                      \\
        settings = (const llh2_settings_t *)(PARSER)->settings;               \\
        if (settings == NULL || settings->NAME == NULL)                       \\
        {                                                                     \\
            err = 0;                                                          \\
            break;                                                            \\
        }                                                                     \\
        err = settings->NAME((PARSER), (START), (LEN));                       \\
        if (err == -1)                                                        \\
        {                                                                     \\
            err = H2PE_USER;                                                  \\
            (PARSER)->reason = "Span callback error in " #NAME;               \\
        }                                                                     \\
    } while (0)


"""

CAPI_EPILOUGE = """


/* API */

void llh2_init(llh2_t *parser, const llh2_settings_t *settings) {
    llh2__internal_init(parser);
    /*  Unlike llhttp we reserve type for handling frames as 
        in the frame type according to RFC 9113 */
    parser->settings = (void *)settings;
}

void llh2_reset(llh2_t *parser) {
    void* data = parser->data;
    llh2_init(parser, (llh2_settings_t*)parser->settings);
    parser->data = data;
}

uint8_t llh2_get_type(llh2_t *parser) {
    return parser->type;
}

uint8_t llh2_get_flags(llh2_t *parser)
{
    return parser->flags;
}

uint32_t llh2_get_length(llh2_t *parser)
{
    return parser->length;
}

uint32_t llh2_get_stream_id(llh2_t *parser)
{
    return parser->stream_id;
}

void llh2_resume(llh2_t *parser)
{
    if (parser->error != H2PE_PAUSED){
        return;
    }
    parser->error = 0;
}

llh2_errno_t llh2_execute(llh2_t *parser, const char *data, size_t len)
{
    return (llh2_errno_t)llh2__internal_execute(parser, data, data + len);
}

const char* llh2_get_type_name(llh2_t* parser){
    switch (parser->type){
        #define __LLH2_TYPE_NAME_CASE(ID, NAME, _) case ID: return #NAME;
        LLH2_FRAME_TYPE_MAP_MAP(__LLH2_TYPE_NAME_CASE)
        #undef __LLH2_TYPE_NAME_CASE
        default: 
            return "UNKNOWN";
    }
}

void llh2_settings_init(llh2_settings_t* settings){
    memset(settings, 0, sizeof(*settings));
};

"""

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


ParserT = TypeVar("ParserT", bound="AbstractParser")


class AbstractParser(LLExt):
    """Base plate for building parsers to make things a bit easier to setup"""

    def __init__(
        self,
        llparse,
        callbacks: dict[str, LLExt] = {},
        fields: dict[str, UIntField | IntField] = {},
    ):
        super().__init__(llparse)
        self.callbacks: dict[str, LLExt] = callbacks
        self.fields = fields

    def uint(
        self,
        field: str,
        ty: Literal["i8", "i16", "i32"],
        err: Error,
        bits: int | None = None,
    ) -> UIntField:
        """creates a new uint property to use"""
        if field in self.fields:
            return self.fields[field]  # should be truthy
        f = UIntField(self.llparse, field, ty, err, bits)
        # just need to holding onto this for writing 
        # the settings structure
        if f'{field}_complete' not in self.callbacks:
            self.callbacks[f"{field}_complete"] = f.on_complete
        return f

    def int(
        self,
        field: str,
        ty: Literal["i8", "i16", "i32"],
        err: Error,
        bits: int | None = None,
    ) -> IntField:
        if field in self.fields:
            return self.fields[field]  # value should be truthy

        f = IntField(self.llparse, field, ty, err, bits)
        
        if f'{field}_complete' not in self.callbacks:
            self.callbacks[f"{field}_complete"] = f.on_complete

        return f

    def do_build(self):
        # Empty
        return ""

    def callback(self, name: str) -> MatchWrap:
        """Writes a callback C Wrapper to use"""
        if name in self.callbacks:
            return self.callbacks[name]
        self.callbacks[name] = MatchWrap(self.llparse, name)
        return self.callbacks[name]

    def span(self, name: str) -> SpanWrap:
        """Writes a span callback C Wrapper to use and adds a on_complete version of that specific callback"""
        if name in self.callbacks:
            return self.callbacks[name]

        sw = self.callbacks[name] = SpanWrap(self.llparse, name)
        self.callbacks[f"{name}_complete"] = sw.complete_cb
        return sw

    def merge(
        self,
        factory: Callable[
            [LLParse, dict[str, LLExt], dict[str, IntField | UIntField]], ParserT
        ],
        next: Node,
    ) -> Node:
        """Merge parsers and build it's part out via creating it
        and utilizing it's callbacks for the final product"""
        result = factory(self.llparse, self.callbacks, self.fields)
        start = result.build(next)
        return start

    @abstractmethod
    def build(self, end: Node) -> Node:
        """Builds a frame parser to work with

        :param end: the node to goto when finished"""


class Padding(AbstractParser):
    def build(self, end: Node) -> Node:
        p = self.llparse

        pad_length = self.uint("pad_length", "i8", Error.CB_ON_PAD_LENGTH, 1)
        start = p.invoke(
            # we can check the byte before consumption to the length
            p.code.match("llh2__before_pad_length"),
            {
                1: p.error(
                    Error.INVALID_LENGTH.value, "pad_length conflicts with length"
                )
            },
        ).otherwise( pad_length.consume(p.consume("pad_length").skipTo(end)))
        # Called before padding has completed to check for invalid lengths
        return self.test("flags", StreamFlags.PADDED, start, end)


class Priority(AbstractParser):
    def build_start(self, end: Node) -> Node:
        """Seperate function incase were dealing with PriorityFrames"""
        p = self.llparse
        stream_weight = self.uint(
            "stream_weight", "i8", Error.CB_ON_STREAM_WEIGHT, 1
        ).consume(end)
        dep_id = self.uint("dependency_id", "i32", Error.CB_ON_DEPENDENCY_ID, 4)

        # Setting exclusive property will be done here...

        start = p.invoke(
            p.code.match("llh2__before_priority"),
            {
                1: p.error(
                    Error.INVALID_LENGTH.value, "Priority data conflicts with length"
                )
            },
        ).otherwise(
            # Set exclusive on post_dependency_id
            dep_id.consume_and_complete_after(
                p.invoke(p.code.match("llh2__post_dependency_id")), stream_weight
            )
        )
        return start

    def build(self, end: Node) -> Node:
        return self.test("flags", StreamFlags.PRIORITY, self.build_start(end), end)


class DataFrame(Padding):
    # 0x00
    def __init__(self, llparse, callbacks = {}, fields = {}):
        super().__init__(llparse, callbacks, fields)
        self.body = self.span('on_body')

    def build(self, end: Node) -> Node:
        p = self.llparse
        before_data_body_start = p.invoke(p.code.match('llh2__before_data_frame_body_start'), 
                                          self.body.start(p.consume('_sub_length').otherwise(self.body.end(Error.CB_ON_BODY_END, end))))
        return super().build(before_data_body_start)


        


class HeadersFrame(Padding):
    # 0x01
    def build(self, end: Node):
        p = self.llparse
        on_body = self.span("on_body")
        return super().build(
            Priority(self.llparse, self.callbacks, self.fields).build(
                on_body.start(
                    p.consume("_sub_length").otherwise(
                        on_body.end(Error.CB_ON_BODY_END, end)
                    )
                )
            )
        )


class PriorityFrame(Priority):
    # 0x02
    def build(self, end: Node):
        # chain directly instead of testing the flags immediately
        return self.build_start(end)


class RstStreamFrame(AbstractParser):
    # 0x03
    def build(self, end: Node) -> Node:
        return self.is_equal(
            "length",
            4,
            self.uint("error_code", "i32", Error.CB_ON_ERROR_CODE).consume(end),
            self.error(Error.INVALID_LENGTH, "RST_STREAM must be a length of 4"),
        )



class SettingsFrame(AbstractParser):
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

    # 0x04

    def __init__(self, llparse: LLParse, callbacks = {}, fields = {}):
        super().__init__(llparse, callbacks, fields)
        llparse.property("i16", "settings_id")
        self.on_settings_id = self.callback('on_settings_id')
        self.on_settings_value = self.uint(
            "settings_value", "i32", Error.CB_ON_SETTINGS_VALUE
        )

        # Strictness will come in a future update

    # end is related to callback on_frame_end
    def build(self, end: Node) -> Node:
        p = self.llparse

        ident = p.node("ident")

        invalid_ident = self.error(
            Error.INVALID_SETTINGS_IDENTIFIER, "settings identifier is invalid"
        )

        # Better strict then sorry...
        # increments by 6 if less than or equal to zero, then stop
        selection: dict[str, int] = {
            "\x00" + chr(v.value): v.value
            for v in SettingsIndentifiers._member_map_.values()
        }

        sub = p.invoke(p.code.match("llh2__settings_subtract"), {0: end}).otherwise(
            ident
        )
        # invalid if it doesn't match '\x00' first since numbers are below 256

        ident.select(
            selection,
            p.invoke(
                p.code.store("settings_id"),
                self.on_settings_id.invoke_pausable(
                    Error.CB_ON_SETTINGS_ID,
                    self.on_settings_value.consume(sub)
                ),
            ),
        ).otherwise(invalid_ident)

        # TODO: In a Strict Mode use aggressive errors against lengths of 0
        # If ACK then ignore otherwise any setting flying in needs parsing
        return self.test("flags", AckFlags.ACK, end, ident)


class PushPromiseFrame(Padding):
    # 0x05
    def __init__(self, llparse, callbacks = {}, fields = {}):
        super().__init__(llparse, callbacks, fields)
        self.on_promise_stream_id = self.uint(
            "promise_stream_id", "i32", Error.CB_ON_PROMISE_STREAM_ID
        )

    def build(self, end: Node):
        p = self.llparse

        validate = p.invoke(
            p.code.match("llh2__before_promise_stream_id_complete"),
            {
                1: self.error(
                    Error.INVALID_PROMISE_STREAM_ID, "invalid promise_stream_id"
                )
            },
        )
        # validate beforehand
        consume = self.on_promise_stream_id.consume_and_complete_after(validate, end)
        return super().build(consume)


class PingFrame(AbstractParser):
    # 0x06
    def build(self, end: Node) -> Node:
        p = self.llparse
        span = self.span("on_body")
        body = span.start(
            p.consume("length").otherwise(span.end(Error.CB_ON_BODY_END, end))
        )
        return self.is_equal(
            "length",
            8,
            body,
            self.error(Error.INVALID_LENGTH, "PING frames must be a length of 8"),
        )


class GoAwayFrame(AbstractParser):
    # 0x07
    def __init__(self, llparse, callbacks={}, fields={}):
        super().__init__(llparse, callbacks, fields)
        self.on_last_stream_id = self.uint(
            "last_stream_id", "i32", Error.CB_ON_LAST_STREAM_ID
        )
        self.on_error_code = self.uint("error_code", "i32", Error.CB_ON_ERROR_CODE)
        self.on_body = self.span("on_body")

    def build(self, end: Node) -> Node:
        p = self.llparse

        self.on_error_code.consume(
            self.on_body.start(
                p.consume("_sub_length").otherwise(self.on_body.end(Error.CB_ON_BODY_END, end))
            )
        )

        return self.on_last_stream_id.consume_and_complete_after(
            p.invoke(
                # subtract sublength by 8 on success
                p.code.match("llh2__check_goaway_frame_length"),
                {1: p.error(Error.INVALID_LENGTH, "Invalid GOAWAY length")},
            ),
            end,
        )


class WindowUpdateFrame(AbstractParser):
    # 0x08
    def __init__(self, llparse, callbacks = {}, fields = {}):
        super().__init__(llparse, callbacks, fields)
        self.on_window_increment = self.uint(
            "window_increment", "i32", Error.CB_ON_WINDOW_INCREMENT
        )

    def build(self, end: Node):
        return self.is_equal("length", 4, self.on_window_increment.consume(end), self.error(Error.INVALID_LENGTH, "Invalid length for WINDOW_UPDATE frame"))


class ContinuationFrame(AbstractParser):
    # 0x09
    def build(self, end: Node) -> Node:
        p = self.llparse
        on_body = self.span("on_body")
        return on_body.start(
            p.consume("length").otherwise(on_body.end(Error.CB_ON_BODY_END, end))
        )


class AltSvcFrame(AbstractParser):
    # 0x0A
    # RFC 7838
    def __init__(self, llparse: LLParse, callbacks={}, fields={}):
        super().__init__(llparse, callbacks, fields)
        self.origin_length = self.uint(
            "altsvc_origin_length", "i16", Error.CB_ON_ALTSVC_ORIGIN_LENGTH
        )
        self.origin_value = self.span("altsvc_origin_value")
        self.field_value = self.span("altsvc_field_value")
        self.before_origin_length_complete = llparse.code.match(
            "llh2__before_origin_length_complete"
        )

    def build(self, end: Node) -> Node:
        p = self.llparse

        before = p.invoke(
            self.before_origin_length_complete,
            {
                1: self.error(
                    Error.INVALID_LENGTH, "length conflicts with ALTSVC origin length"
                )
            },
        )
        # subtract length from altsvc_origin_length to _sub_length in llh2__before_origin_length_complete
        field_value_start = self.field_value.start(
            p.consume("_sub_length").otherwise(
                self.field_value.end(Error.CB_ON_ALTSVC_FIELD_VALUE, end)
            )
        )

        origin_value_start = self.origin_value.start(
            p.consume("altsvc_origin_length").otherwise(
                self.origin_value.end(
                    Error.CB_ON_ALTSVC_ORIGIN_VALUE, field_value_start
                )
            )
        )
        return self.origin_length.consume_and_complete_after(before, origin_value_start)


class H2Parser(AbstractParser):
    """Main Parser"""
    def __init__(self, llparse, callbacks = {}, fields = {}):
        super().__init__(llparse, callbacks, fields)
        self.on_reset = self.callback('on_reset')
        self.on_frame_start = self.callback('on_frame_start')
        self.on_frame_end = self.callback('on_frame_end')


    def pause(self, msg: str, Next: Node | None = None):
        res = self.llparse.pause(Error.PAUSED.value, msg)
        if Next != None:
            res.otherwise(Next)
        return res

    def consume_unknown_frame_body_ext(self, otherwise:Node):
        on_body = self.span('on_body')
        stop = on_body.end(Error.CB_ON_BODY_END, otherwise)
        return on_body.start(self.llparse.consume('length').otherwise(stop))
       

    def check_frame_types(self, next:Node):
        """Checks if a subparser can parse a certain frame type 
        runs on_body span directly if not found if not in strict mode
        otherwise parser will throw an error or pause"""
        load_type = self.load('type', 
            {
                FrameType.DATA: self.merge(DataFrame, next),
                FrameType.HEADERS: self.merge(HeadersFrame, next),
                FrameType.PRIORITY: self.merge(PriorityFrame, next),
                FrameType.RST_STREAM: self.merge(RstStreamFrame, next),
                FrameType.SETTINGS: self.merge(SettingsFrame, next),
                FrameType.PUSH_PROMISE: self.merge(PushPromiseFrame, next),
                FrameType.PING: self.merge(PingFrame, next),
                FrameType.GOAWAY: self.merge(GoAwayFrame, next),
                FrameType.WINDOW_UPDATE: self.merge(WindowUpdateFrame, next),
                FrameType.CONTINUATION: self.merge(ContinuationFrame, next),
                FrameType.ALTSVC:self.merge(AltSvcFrame, next)
            }
        )
        # TODO: Strict Mode should Throw an error if type is unknown
        load_type.otherwise(self.consume_unknown_frame_body_ext(next))
        return load_type

    def build(self):
        p = self.llparse
        p.property("ptr", "settings")
        # sperate node to safely hook stream_id later, Peephole should clear it beforehand
        pre_stream_id = p.node('pre_stream_id')
        
        length = self.uint('length', 'i32', Error.CB_ON_LENGTH, 3) # i24 in reality hence 3 bits instead of 4
        stream_id = self.int("stream_id", 'i32', Error.CB_ON_STREAM_ID, 4)
        ty = self.uint('type', 'i8', Error.CB_ON_TYPE)
        flags = self.uint('flags', "i8", Error.CB_ON_FLAG)
        p.property('i8', 'is_exclusive')


        # _sub_length used for subtraction (internal) 
        # added so that a subparser doesn't need to be introduced to the build
        # this way were able to parse all HTTP/2 Frames that are publically in 
        # use this value should not be exposed to the developer or users end 
        # unless absolutely required
        
        p.property("i32", "_sub_length")  
        
      
        consume_length = length.consume(ty.consume(flags.consume(pre_stream_id)))

        
        start = self.on_frame_start.invoke_pausable(Error.CB_ON_FRAME_START, consume_length)

        _exit = p.node('exit')
        

        on_reset = self.on_reset.invoke(
                {
                    Reset.PAUSE.value: self.pause("paused parser at on_reset", start),
                    Reset.EXIT.value: _exit,
                },
            ).otherwise(self.error(Error.CB_ON_RESET, "on_reset callback error"))
        
        end = self.on_frame_end.invoke_pausable(Error.CB_ON_FRAME_END, on_reset)
        pre_stream_id.otherwise(stream_id.consume(self.check_frame_types(end)))

        _exit.skipTo(_exit)
        return start

    def do_build(self):
        """writes settings and other important data down"""
       
        # DUE To LLH2 being a little bit bigger than llhttp in some ways
        # autogenerating the CWrapper portion became nessesary
        code = CHEADERS_CWRAPPER_PROLOUGE
        spans = [i for i in self.callbacks.values() if isinstance(i, SpanWrap)]
        cbs = [i for i in self.callbacks.values() if isinstance(i, MatchWrap)]

        code += '\n  /* DATA CALLBACKS */\n\n'
        for s in spans:
            code += s.cb_property + '\n'
        
        code += '\n  /* CALLBACKS */\n\n'
        for cb in cbs:
            code += cb.cb_property + '\n'
        
        code += '};\n'

        code += CHEADERS_CWRAPPER_EPILOUGE

        return code

    def build_api(self):
        """Build all callbacks that can at least be safely automated
        rest are left to http2.c to handle"""
        code = CAPI_PROLOUGE + "\n"

        code += "/* AUTO GENERATED PLEASE DO NOT EDIT!!! */\n\n"
        
        for cbs in self.callbacks.values():
            code += cbs.do_build() + '\n\n'
        return code + CAPI_EPILOUGE

    def build_test(self):
        """Build all callbacks that can at least be safely automated
        rest are left to http2.c to handle"""
        code = ""
        # code += "/* AUTO GENERATED CALLBACKS PLEASE DO NOT EDIT!!! */\n\n"
        
        for cbs in self.callbacks.values():
            code += cbs.do_cython_setter()
  
        return code



if __name__ == "__main__":
    llh2 = LLParse("llh2__internal")
    p = H2Parser(llh2)
    root = p.build()
    src = llh2.build(
        root, header_name="llh2", headerGuard="INCLUDE_LLH2_ITSELF_H_",
        # debug='llh2__internal_debug'
    )
    build_folder = Path("src")
    include = Path("include")


    (build_folder / "llh2.c").write_text(src.c)
    (include / "llh2.h").write_text(buildCHeaders() + src.header + '\n' + p.do_build())
    (build_folder / "api.c").write_text(p.build_api())


