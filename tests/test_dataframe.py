from h2parser import Parser
from hyperframe import frame

from dataclasses import dataclass, field
from enum import IntEnum, IntFlag

from io import BytesIO


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


@dataclass
class DataFrame:
    flags: int = 0
    pad_length: int = 0
    data: bytearray = field(default_factory=bytearray)
    length: int = 0

    def clear(self):
        self.flags = 0
        self.pad_length = 0
        self.data = bytearray()
        self.length = 0

    @property
    def body_len(self):
        return len(self.data)


class DataFrameParser(Parser):
    frame = DataFrame()

    def on_body(self, value: bytes):
        self.frame.data += value

    def on_frame_start(self):
        self.frame.clear()

    def on_flags_complete(self):
        self.frame.flags = self.flags

    def on_pad_length_complete(self):
        self.frame.pad_length = self.pad_length

    def on_length_complete(self):
        self.frame.length = self.length


class TestDataFrame:
    # mirror for hyperframe which is most of this throught the code
    payload = b"\x00\x00\x08\x00\x01\x00\x00\x00\x01testdata"
    payload_with_padding = (
        b"\x00\x00\x13\x00\x09\x00\x00\x00\x01\x0atestdata" + b"\0" * 10
    )

    def test_data_frame_payload_no_padding(self):
        parser = DataFrameParser()
        p = parser.exec(self.payload)
        frame = parser.frame
        # should be paused when body completes
        assert p == 1
        assert frame.flags & StreamFlags.END_STREAM
        assert frame.pad_length == 0
        assert bytes(frame.data) == b"testdata"
        assert frame.body_len == 8
        assert frame.length == 8
        assert parser.type == FrameType.DATA

    def test_data_frame_payload_no_padding_as_stream(self):
        parser = DataFrameParser()
        p = parser.exec(b"\x00\x00\x08\x00\x01\x00\x00\x00")
        p = parser.exec(b"\x01testdata")

        frame = parser.frame

        assert p == 1
        assert frame.flags & StreamFlags.END_STREAM
        assert frame.pad_length == 0
        assert bytes(frame.data) == b"testdata"
        assert frame.body_len == 8
        assert frame.length == 8
        assert parser.type == FrameType.DATA

    def test_data_frame_payload_padding(self):
        parser = DataFrameParser()
        p = parser.exec(self.payload_with_padding)
        assert p == 1
        f = parser.frame
        assert f.flags & (StreamFlags.END_STREAM | StreamFlags.PADDED)
        assert f.pad_length == 10
        assert bytes(f.data) == b"testdata"
        # body length is unaffected by the actaul length with the other data
        # We subtract 1 from 19 and get 18 and then subtract the pad_length to get 9
        assert f.body_len == 8
        assert f.length == 19
