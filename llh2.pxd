# cython: language_level = 3
# if you plan to use in cython simply copying this code or file should do the job

from libc.stdint cimport int32_t as int32_t
from libc.stdint cimport uint32_t as uint32_t 
from libc.stdint cimport uint8_t as uint8_t   

cdef extern from "build/llh2.h" nogil:
    enum llh2_reset_flag:
        H2RF_ERROR = -1
        H2RF_PAUSE = 0
        H2RF_EXIT = 1
    ctypedef llh2_reset_flag llh2_reset_flag_t
    enum llh2_errno:
        H2PE_OK = 0
        H2PE_PAUSED = 1
        H2PE_LENGTH_OVERFLOW = 2
        H2PE_STREAM_ID_OVERFLOW = 3
        H2PE_CB_ON_FRAME_START = 4
        H2PE_CB_ON_FRAME_END = 5
        H2PE_CB_ON_LENGTH = 6
        H2PE_CB_ON_STREAM_ID = 7
        H2PE_CB_ON_TYPE = 8
        H2PE_CB_ON_FLAG = 9
        H2PE_CB_ON_BODY_START = 10
        H2PE_CB_ON_BODY_END = 11
        H2PE_CB_ON_RESET = 12
        H2PE_EXIT = 13
        H2PE_USER = 14
    ctypedef llh2_errno llh2_errno_t

    ctypedef struct llh2__internal_s:
        int32_t _index
        void* _span_pos0
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
    ctypedef llh2__internal_s llh2__internal_t
    int llh2__internal_init(llh2__internal_t*)
    int llh2__internal_execute(llh2__internal_t*, const char*, const char*)

cdef extern from "native/api.h"    
    ctypedef llh2__internal_t llh2_t
    ctypedef struct llh2_settings_s:
        llh2_data_cb on_body
        llh2_cb on_frame_start
        llh2_cb on_frame_end
        llh2_cb on_length
        llh2_cb on_type
        llh2_cb on_flags
        llh2_cb on_body_start
        llh2_cb on_body_end
        llh2_cb on_reset
    ctypedef llh2_settings_s llh2_settings_t

    ctypedef int (*llh2_data_cb)(llh2_t*, const char *at, size_t length) except -1 with gil
    ctypedef int (*llh2_cb)(llh2_t*) except -1 with gil


    void llh2_init(llh2_t* parser, const llh2_settings_t* settings)
    void llh2_reset(llh2_t* parser)


    uint8_t llh2_get_type(llh2_t* parser)


    uint8_t llh2_get_flags(llh2_t* parser)

    uint32_t llh2_get_length(llh2_t* parser)

    uint32_t llh2_get_stream_id(llh2_t* parser)

    void llh2_resume(llh2_t* parser)

    llh2_errno_t llh2_execute(llh2_t* parser, const char* data, size_t len)
