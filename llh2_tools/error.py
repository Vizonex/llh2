from enum import IntEnum, auto


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

    CB_ON_SETTINGS_ID = auto()
    CB_ON_SETTINGS_VALUE = auto()

    CB_ON_PROMISE_STREAM_ID = auto()

    CB_ON_LAST_STREAM_ID = auto()
    CB_ON_ERROR_CODE = auto()
    CB_ON_WINDOW_INCREMENT = auto()

    CB_ON_PAD_LENGTH = auto()

    CB_ON_DEPENDENCY_ID = auto()
    CB_ON_STREAM_WEIGHT = auto()

    CB_ON_ALTSVC_ORIGIN_LENGTH = auto()
    CB_ON_ALTSVC_ORIGIN_VALUE = auto()
    CB_ON_ALTSVC_FIELD_VALUE = auto()

    # Errors
    INVALID_SETTINGS_IDENTIFIER = auto()
    INVALID_LENGTH = auto()
    INVALID_PAD_LENGTH = auto()
    INVALID_PROMISE_STREAM_ID = auto()
    INVALID_PUSH_PROMISE_BODY = auto()
    INVALID_HEADERS_FRAME = auto()

    # User Chose to Exit from the loop after parsing a frame
    EXIT = auto()
    USER = auto()
