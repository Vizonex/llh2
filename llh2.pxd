from libc.stdint cimport int32_t as int32_t
from libc.stdint cimport uint16_t as uint16_t
from libc.stdint cimport uint32_t as uint32_t
from libc.stdint cimport uint8_t as uint8_t

cdef extern from "include/llh2.h":
    enum llh2_reset_flag:
        H2RF_ERROR = -1
        H2RF_PAUSE = 0
        H2RF_EXIT = 1
    ctypedef llh2_reset_flag llh2_reset_flag_t
    enum llh2_errno:
        H2PE_OK = 0
        H2PE_PAUSED = 1
        H2PE_CB_ON_FRAME_START = 2
        H2PE_CB_ON_FRAME_END = 3
        H2PE_CB_ON_LENGTH = 4
        H2PE_CB_ON_STREAM_ID = 5
        H2PE_CB_ON_TYPE = 6
        H2PE_CB_ON_FLAG = 7
        H2PE_CB_ON_BODY_START = 8
        H2PE_CB_ON_BODY_END = 9
        H2PE_CB_ON_RESET = 10
        H2PE_CB_ON_SETTINGS_ID = 11
        H2PE_CB_ON_SETTINGS_VALUE = 12
        H2PE_CB_ON_PROMISE_STREAM_ID = 13
        H2PE_CB_ON_LAST_STREAM_ID = 14
        H2PE_CB_ON_ERROR_CODE = 15
        H2PE_CB_ON_WINDOW_INCREMENT = 16
        H2PE_CB_ON_PAD_LENGTH = 17
        H2PE_CB_ON_DEPENDENCY_ID = 18
        H2PE_CB_ON_STREAM_WEIGHT = 19
        H2PE_CB_ON_ALTSVC_ORIGIN_LENGTH = 20
        H2PE_CB_ON_ALTSVC_ORIGIN_VALUE = 21
        H2PE_CB_ON_ALTSVC_FIELD_VALUE = 22
        H2PE_INVALID_SETTINGS_IDENTIFIER = 23
        H2PE_INVALID_LENGTH = 24
        H2PE_INVALID_PAD_LENGTH = 25
        H2PE_INVALID_PROMISE_STREAM_ID = 26
        H2PE_EXIT = 27
        H2PE_USER = 28
    ctypedef llh2_errno llh2_errno_t
    enum llh2_misc_flags:
        H2MF_NONE = 0
        H2MF_ACK = 1
    ctypedef llh2_misc_flags llh2_misc_flags_t
    enum llh2_stream_flags:
        H2SF_NONE = 0
        H2SF_END_STREAM = 1
        H2SF_END_HEADERS = 4
        H2SF_PADDED = 8
        H2SF_PRIORITY = 32
    ctypedef llh2_stream_flags llh2_stream_flags_t
    enum llh2_frame_type:
        H2FT_DATA = 0
        H2FT_HEADERS = 1
        H2FT_PRIORITY = 2
        H2FT_RST_STREAM = 3
        H2FT_SETTINGS = 4
        H2FT_PUSH_PROMISE = 5
        H2FT_PING = 6
        H2FT_GOAWAY = 7
        H2FT_WINDOW_UPDATE = 8
        H2FT_CONTINUATION = 9
        H2FT_ALTSVC = 10
    ctypedef llh2_frame_type llh2_frame_type_t
    
    struct llh2__internal_s:
        int32_t _index
        void* _span_pos0
        void* _span_cb0
        int32_t error
        const char* reason
        const char* error_pos
        void* data
        void* _current
        void* settings
        uint32_t length
        uint32_t stream_id
        uint8_t type
        uint8_t flags
        uint8_t is_exclusive
        uint32_t _sub_length
        uint8_t pad_length
        uint8_t stream_weight
        uint32_t dependency_id
        uint32_t error_code
        uint16_t settings_id
        uint32_t settings_value
        uint32_t promise_stream_id
        uint32_t last_stream_id
        uint32_t window_increment
        uint16_t altsvc_origin_length
        
    ctypedef llh2__internal_s llh2__internal_t
    int llh2__internal_init(llh2__internal_t*)
    int llh2__internal_execute(llh2__internal_t*, const char*, const char*)
    ctypedef llh2__internal_t llh2_t
    
    ctypedef int (*llh2_data_cb)(llh2_t*, const char*, size_t) noexcept
    ctypedef int (*llh2_cb)(llh2_t*) noexcept
    struct llh2_settings_s:
        llh2_data_cb on_body
        llh2_data_cb altsvc_origin_value
        llh2_data_cb altsvc_field_value
        llh2_cb on_reset
        llh2_cb on_frame_start
        llh2_cb on_frame_end
        llh2_cb on_length_complete
        llh2_cb on_stream_id_complete
        llh2_cb on_type_complete
        llh2_cb on_flags_complete
        llh2_cb on_body_complete
        llh2_cb on_pad_length_complete
        llh2_cb on_stream_weight_complete
        llh2_cb on_dependency_id_complete
        llh2_cb on_error_code_complete
        llh2_cb on_settings_id
        llh2_cb on_settings_value_complete
        llh2_cb on_promise_stream_id_complete
        llh2_cb on_last_stream_id_complete
        llh2_cb on_window_increment_complete
        llh2_cb on_altsvc_origin_length_complete
        llh2_cb altsvc_origin_value_complete
        llh2_cb altsvc_field_value_complete

    ctypedef llh2_settings_s llh2_settings_t

    void llh2_init(llh2_t*, llh2_settings_t*)
    void llh2_reset(llh2_t*)
    uint8_t llh2_get_type(llh2_t*)
    uint8_t llh2_get_flags(llh2_t*)
    uint32_t llh2_get_length(llh2_t*)
    uint32_t llh2_get_stream_id(llh2_t*)
    void llh2_resume(llh2_t*)
    llh2_errno_t llh2_execute(llh2_t*, const char*, size_t)
    const char* llh2_get_type_name(llh2_t*)

    void llh2_settings_init(llh2_settings_t* settings)
    
