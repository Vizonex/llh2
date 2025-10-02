from h2parser import Parser
from enum import IntEnum, IntFlag
from dataclasses import dataclass, field
import pytest


# Majority of these were ripped from hyperframe so that
# new testdata wouldn't be nessesary to hunt for.


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


@dataclass
class PriorityFrame:
    stream_id: int = 0
    exclusive: bool = False
    depends_on: int = 0
    stream_weight: int = 0
    pad_length: int = 0
    length: int = 0

    def clear(self):
        self.stream_id = 0
        self.exclusive = False
        self.depends_on = 0
        self.stream_weight = 0
        self.pad_length = 0


class FrameParser(Parser):
    frame = PriorityFrame()

    def on_frame_start(self):
        self.frame.clear()

    def on_flags_complete(self):
        self.frame.flags = self.flags

    def on_pad_length_complete(self):
        self.frame.pad_length = self.pad_length

    def on_length_complete(self):
        self.frame.length = self.length

    def on_dependency_id_complete(self):
        assert self.dependency_id == 4
        self.frame.depends_on = self.dependency_id
        self.frame.exclusive = bool(self.is_exclusive)

    def on_stream_weight_complete(self):
        print(f"STREAM {self.stream_weight}")
        self.frame.stream_weight = self.stream_weight

    def on_stream_id_complete(self):
        self.frame.stream_id = self.stream_id


class BodyFrameParser(Parser):
    def __init__(self):
        self.body = bytearray()
        self.data = b""
        super().__init__()

    def on_length_complete(self):
        print(f"[LENGTH]: {self.length}")
        self._length = self.length
        # return super().on_length_complete()

    # def clear(self):
    #     self.data = b""
    #     self.body.clear()

    def on_body(self, value: bytes):
        self.body.extend(value)

    def on_body_complete(self):
        # lazy conversion tactic
        print(f"{self.body}")
        self.data = bytes(self.body)
        print(f"{self.data}")
    
    def on_frame_end(self):
        print(f"{self.body}")
        self.data = bytes(self.body)
        print(f"{self.data}")



class TestPriorityFrame:
    payload = b"\x00\x00\x05\x02\x00\x00\x00\x00\x01\x80\x00\x00\x04\x40"

    def test_priority_frame_with_all_data(self):
        p = FrameParser()
        ret = p.exec(self.payload)
        assert ret == 1

        f = p.frame
        assert p.type == FrameType.PRIORITY
        assert f.depends_on == 4
        assert f.stream_weight == 64
        assert f.exclusive is True
        assert f.length == 5


class TestRstStreamFrame:
    payload = b"\x00\x00\x04\x03\x00\x00\x00\x00\x01\x00\x00\x01\xa4"

    def test_reset_stream_frame(self):
        # No hooks required with this one...
        p = Parser()
        ret = p.exec(self.payload)
        assert ret == 1

        assert p.flags == 0
        assert p.error_code == 420
        assert p.length == 4


class SettingsParser(Parser):
    data = {}

    def on_settings_value_complete(self):
        self.data[self.settings_id] = self.settings_value


class TestSettingsFrame:
    payload = (
        # Ripped from hyperframe
        b"\x00\x00\x2a\x04\x01\x00\x00\x00\x00"  # Frame header
        + b"\x00\x01\x00\x00\x10\x00"  # HEADER_TABLE_SIZE
        + b"\x00\x02\x00\x00\x00\x00"  # ENABLE_PUSH
        + b"\x00\x03\x00\x00\x00\x64"  # MAX_CONCURRENT_STREAMS
        + b"\x00\x04\x00\x00\xff\xff"  # INITIAL_WINDOW_SIZE
        + b"\x00\x05\x00\x00\x40\x00"  # MAX_FRAME_SIZE
        + b"\x00\x06\x00\x00\xff\xff"  # MAX_HEADER_LIST_SIZE
        + b"\x00\x08\x00\x00\x00\x01"  # ENABLE_CONNECT_PROTOCOL
    )
    settings = {
        1: 4096,
        2: 0,
        3: 100,
        4: 65535,
        5: 16384,
        6: 65535,
        8: 1,
    }

    def test_settings_frame(self):
        data = self.payload[:4] + b"\x00" + self.payload[5:]
        p = SettingsParser()
        ret = p.exec(data)
        assert ret == 1
        assert p.data == self.settings
        assert p.length == 42


class AckFlags(IntFlag):
    NONE = 0x00
    ACK = 0x01


class StreamFlags(IntFlag):
    NONE = 0x00  # 0
    END_STREAM = 0x01  # 1
    END_HEADERS = 0x04  # 4
    PADDED = 0x08  # 8
    PRIORITY = 0x20  # 32


class TestPushPromise:
    payload = (
        b"\x00\x00\x0f\x05\x04\x00\x00\x00\x01" + b"\x00\x00\x00\x04" + b"hello world"
    )
    payload_with_padding = (
        b"\x00\x00\x17\x05\x0c\x00\x00\x00\x01"
        + b"\x07\x00\x00\x00\x04"
        + b"hello world"
        + b"padding"
    )

    def test_push_promise_frame(self):
        p = BodyFrameParser()
        ret = p.exec(self.payload)
        # payload
        assert ret, "Parser should've been paused"  # should be paused which is 1

        assert p.flags & StreamFlags.END_HEADERS, "END_HEADERS should be set"
        assert p.promise_stream_id == 4
        assert p.data == b"hello world"
        assert p.length == 15

    def test_push_promise_frame_with_padding(self):
        p = BodyFrameParser()
        ret = p.exec(self.payload_with_padding)
        # payload
        assert ret, "Parser should be paused"  # should be paused which is 1
        assert p.flags & StreamFlags.END_HEADERS, "END_HEADERS should be set"
        assert p.promise_stream_id == 4
        assert p.data == b"hello world"
        assert p.length == 23


class TestPingFrame:
    payload = b"\x00\x00\x08\x06\x01\x00\x00\x00\x00\x01\x02\x00\x00\x00\x00\x00\x00"

    def test_pring_frame(self):
        p = BodyFrameParser()
        ret = p.exec(self.payload)
        assert ret, "Parser should be paused"
        assert p.flags & AckFlags.ACK
        assert p.data == b"\x01\x02\x00\x00\x00\x00\x00\x00"
        assert p._length == 8


class TestGoAwayFrame:
    payload = (
        b"\x00\x00\x0d\x07\x00\x00\x00\x00\x00"  # Frame header
        + b"\x00\x00\x00\x40"  # Last Stream ID
        + b"\x00\x00\x00\x20"  # Error Code
        + b"hello"  # Additional data
    )
    payload_no_addtional_data = (
        b"\x00\x00\x08\x07\x00\x00\x00\x00\x00"  # Frame header
        + b"\x00\x00\x00\x40"  # Last Stream ID
        + b"\x00\x00\x00\x20"  # Error Code
        + b""  # Additional data
    )

    def test_goaway_frame(self):
        p = BodyFrameParser()
        ret = p.exec(self.payload)
        assert ret, "Parser should be paused"
        assert p.type == FrameType.GOAWAY
        assert not p.flags  # 0
        assert p.data == b"hello"
        assert p._length == 13

    def test_goaway_frame_no_additional_data(self):
        p = BodyFrameParser()
        ret = p.exec(self.payload_no_addtional_data)
        assert ret, "Parser should be paused"
        assert not p.flags  # 0
        assert p.data == b""
        assert p._length == 8


class TestWindowUpdateFrame:
    payload = b"\x00\x00\x04\x08\x00\x00\x00\x00\x00\x00\x00\x02\x00"

    def test_windowupdate_frame(self):
        p = Parser()
        ret = p.exec(self.payload)
        assert ret, "Parser should be paused"
        assert p.type == FrameType.WINDOW_UPDATE
        assert not p.flags  # 0
        assert p.window_increment == 512
        assert p.length == 4


class TestHeadersFrame:
    payload = b"\x00\x00\x0b\x01\x05\x00\x00\x00\x01" + b"hello world"
    payload_with_priority = (
        b"\x00\x00\x05\x01\x20\x00\x00\x00\x01" + b"\x80\x00\x00\x04\x40"
    )

    def test_headersframe(self):
        p = BodyFrameParser()
        ret = p.exec(self.payload)
        assert ret, "Parser should be paused"
        assert p.flags & (StreamFlags.END_STREAM | StreamFlags.END_HEADERS)
        assert p.data == b"hello world"
        assert p._length == 11

    def test_headersframe_with_priority(self):
        p = BodyFrameParser()
        ret = p.exec(self.payload_with_priority)
        assert ret, "Parser should be paused"
        # assert p.flags & (StreamFlags.END_STREAM | StreamFlags.END_HEADERS)
        assert p.data == b""
        assert p.dependency_id == 4
        assert p.stream_weight == 64
        assert p.is_exclusive == 1
        assert p._length == 5


class TestContinuationFrame:
    payload = b"\x00\x00\x0b\x09\x04\x00\x00\x00\x01hello world"

    def test_continuation_frame(self):
        p = BodyFrameParser()
        ret = p.exec(self.payload)
        assert ret, "Parser should be paused"
        assert p._length == 11
        assert p.flags & StreamFlags.END_HEADERS
        assert p.data == b"hello world"


class AltSvcParser(Parser):
    def __init__(self):
        super().__init__()
        self.field_body = bytearray()
        self.field = b""

        self.origin_body = bytearray()
        self.origin = b""
        self._length = 0

    def on_length_complete(self):
        self._length = self.length

    def on_altsvc_origin_length_complete(self):
        print(self.altsvc_origin_length)
        # return super().on_altsvc_origin_length_complete()

    def altsvc_origin_value(self, value):
        self.origin_body.extend(value)

    def altsvc_origin_value_complete(self):
        self.origin = bytes(self.origin_body)

    def altsvc_field_value(self, value):
        self.field_body.extend(value)

    def altsvc_field_value_complete(self):
        self.field = bytes(self.field_body)


class TestAltSvcFrame:
    payload_with_origin = (
        b"\x00\x00\x31"  # Length
        b"\x0a"  # Type
        b"\x00"  # Flags
        b"\x00\x00\x00\x00"  # Stream ID
        b"\x00\x0b"  # Origin len
        b"example.com"  # Origin
        b'h2="alt.example.com:8000", h2=":443"'  # Field Value
    )
    payload_without_origin = (
        b"\x00\x00\x13"  # Length
        b"\x0a"  # Type
        b"\x00"  # Flags
        b"\x00\x00\x00\x01"  # Stream ID
        b"\x00\x00"  # Origin len
        b""  # Origin
        b'h2=":8000"; ma=60'  # Field Value
    )

    def decode_frame(self, payload: bytes):
        p = AltSvcParser()
        ret = p.exec(payload)
        assert ret, "Parser should be paused"
        return p

    def test_payload_with_origin(self):
        r = self.decode_frame(self.payload_with_origin)
        assert r.origin == b"example.com"
        assert r.field == b'h2="alt.example.com:8000", h2=":443"'
        assert r._length == 49
        assert r.stream_id == 0

    def test_payload_without_origin(self):
        r = self.decode_frame(self.payload_without_origin)
        assert r.origin == b""
        assert r.field == b'h2=":8000"; ma=60'
        assert r._length == 19
        assert r.stream_id == 1
