from h2parser import Parser
from enum import IntEnum
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


class TestPriorityFrame:
    payload = b"\x00\x00\x05\x02\x00\x00\x00\x00\x01\x80\x00\x00\x04\x40"

    def test_priority_frame_with_all_data(self):
        p = FrameParser()
        ret = p.exec(self.payload)
        assert ret == 1

        f = p.frame
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
