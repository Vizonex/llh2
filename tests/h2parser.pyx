
#cython: language_level=3

from __future__ import print_function
from typing import Optional

from cpython.mem cimport PyMem_Malloc, PyMem_Free
from cpython.buffer cimport PyObject_GetBuffer, PyBuffer_Release, PyBUF_SIMPLE
from cpython.bytes cimport PyBytes_FromStringAndSize
from cpython.object cimport PyObject
from cpython.memoryview cimport PyMemoryView_FromObject, PyMemoryView_GET_BUFFER

# cython plans to deprecate cimporting from just cpython 
# so this is where I choose to deviate.
from cpython.memoryview cimport PyMemoryView_Check, PyMemoryView_GET_BUFFER

from llh2 cimport *

# bridge is used for pytest and hyperframe
# inspired by aiohttp's _http_parser module and magicstack's http-tools library for binding llhttp

cdef extern from "Python.h":
    object PyObject_CallMethodOneArg(object obj, object name, object arg)
    

cdef class Parser:
    cdef llh2_t* _cparser
    cdef llh2_settings_t* _csettings
    cdef bint _bench
    cdef object _exception
    cdef Py_buffer py_buf

    def __cinit__(self):
        self._cparser = <llh2_t*> \
                                PyMem_Malloc(sizeof(llh2_t))
        if self._cparser is NULL:
            raise MemoryError

        self._csettings = <llh2_settings_t*> \
                                PyMem_Malloc(sizeof(llh2_settings_t))
        if self._csettings is NULL:
            raise MemoryError
        
        self._init()
        self._exception = None

    cdef int _on_reset(self):
        cdef object i
        try:
            if i := self.on_reset():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_frame_start(self):
        cdef object i
        try:
            if i := self.on_frame_start():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_frame_end(self):
        cdef object i
        try:
            if i := self.on_frame_end():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_length_complete(self):
        cdef object i
        try:
            if i := self.on_length_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_stream_id_complete(self):
        cdef object i
        try:
            if i := self.on_stream_id_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_type_complete(self):
        cdef object i
        try:
            if i := self.on_type_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_flags_complete(self):
        cdef object i
        try:
            if i := self.on_flags_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_body(self, bytes ob):
        cdef object i
        try:
            if i := self.on_body(ob):
              return <int>i
            return 0 
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_body_complete(self):
        cdef object i
        try:
            if i := self.on_body_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_pad_length_complete(self):
        cdef object i
        try:
            if i := self.on_pad_length_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_stream_weight_complete(self):
        cdef object i
        try:
            if i := self.on_stream_weight_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_dependency_id_complete(self):
        cdef object i
        try:
            if i := self.on_dependency_id_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_error_code_complete(self):
        cdef object i
        try:
            if i := self.on_error_code_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_settings_id(self):
        cdef object i
        try:
            if i := self.on_settings_id():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_settings_value_complete(self):
        cdef object i
        try:
            if i := self.on_settings_value_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_promise_stream_id_complete(self):
        cdef object i
        try:
            if i := self.on_promise_stream_id_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_last_stream_id_complete(self):
        cdef object i
        try:
            if i := self.on_last_stream_id_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_window_increment_complete(self):
        cdef object i
        try:
            if i := self.on_window_increment_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _on_altsvc_origin_length_complete(self):
        cdef object i
        try:
            if i := self.on_altsvc_origin_length_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _altsvc_origin_value(self, bytes ob):
        cdef object i
        try:
            if i := self.altsvc_origin_value(ob):
              return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _altsvc_origin_value_complete(self):
        cdef object i
        try:
            if i := self.altsvc_origin_value_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef int _altsvc_field_value(self, bytes ob):
        cdef object i
        try:
            if i := self.altsvc_field_value(ob):
              return <int>i
            return 0

        except Exception as e:
            self._exception = e
            return -1

    cdef int _altsvc_field_value_complete(self):
        cdef object i
        try:
            if i := self.altsvc_field_value_complete():
               return <int>i
            return 0
        except Exception as e:
            self._exception = e
            return -1

    cdef _init(self):
        llh2_settings_init(self._csettings)
        
        if hasattr(self, "on_reset"):
            self._csettings.on_reset = cb_on_reset

        if hasattr(self, "on_frame_start"):
            self._csettings.on_frame_start = cb_on_frame_start

        if hasattr(self, "on_frame_end"):
            self._csettings.on_frame_end = cb_on_frame_end

        if hasattr(self, "on_length_complete"):
            self._csettings.on_length_complete = cb_on_length_complete

        if hasattr(self, "on_stream_id_complete"):
            self._csettings.on_stream_id_complete = cb_on_stream_id_complete

        if hasattr(self, "on_type_complete"):
            self._csettings.on_type_complete = cb_on_type_complete

        if hasattr(self, "on_flags_complete"):
            self._csettings.on_flags_complete = cb_on_flags_complete

        if hasattr(self, "on_body"):
            self._csettings.on_body = cb_on_body

        if hasattr(self, "on_body_complete"):
            self._csettings.on_body_complete = cb_on_body_complete

        if hasattr(self, "on_pad_length_complete"):
            self._csettings.on_pad_length_complete = cb_on_pad_length_complete

        if hasattr(self, "on_stream_weight_complete"):
            self._csettings.on_stream_weight_complete = cb_on_stream_weight_complete

        if hasattr(self, "on_dependency_id_complete"):
            self._csettings.on_dependency_id_complete = cb_on_dependency_id_complete

        if hasattr(self, "on_error_code_complete"):
            self._csettings.on_error_code_complete = cb_on_error_code_complete

        if hasattr(self, "on_settings_id"):
            self._csettings.on_settings_id = cb_on_settings_id

        if hasattr(self, "on_settings_value_complete"):
            self._csettings.on_settings_value_complete = cb_on_settings_value_complete

        if hasattr(self, "on_promise_stream_id_complete"):
            self._csettings.on_promise_stream_id_complete = cb_on_promise_stream_id_complete

        if hasattr(self, "on_last_stream_id_complete"):
            self._csettings.on_last_stream_id_complete = cb_on_last_stream_id_complete

        if hasattr(self, "on_window_increment_complete"):
            self._csettings.on_window_increment_complete = cb_on_window_increment_complete

        if hasattr(self, "on_altsvc_origin_length_complete"):
            self._csettings.on_altsvc_origin_length_complete = cb_on_altsvc_origin_length_complete

        if hasattr(self, "altsvc_origin_value"):
            self._csettings.altsvc_origin_value = cb_altsvc_origin_value

        if hasattr(self, "altsvc_origin_value_complete"):
            self._csettings.altsvc_origin_value_complete = cb_altsvc_origin_value_complete

        if hasattr(self, "altsvc_field_value"):
            self._csettings.altsvc_field_value = cb_altsvc_field_value

        if hasattr(self, "altsvc_field_value_complete"):
            self._csettings.altsvc_field_value_complete = cb_altsvc_field_value_complete

        llh2_init(self._cparser, self._csettings)
        self._cparser.data = <void*>(self)

    def reset(self):
        llh2_reset(self._cparser)
    
    def resume(self):
        llh2_resume(self._cparser)

    def exec(self, object obj):
        cdef Py_buffer* buf = &self.py_buf
        cdef llh2_errno_t err
        
        #XXX: GetBuffer can sometimes report the wrong length leading to crash
        if PyObject_GetBuffer(obj, buf, PyBUF_SIMPLE) < 0:
            raise
        print(f"execute {<char*>buf.buf} {buf.len}")
        err = llh2_execute(self._cparser, <char*>buf.buf, buf.len)
        print("release")

        if err != H2PE_PAUSED or err != H2PE_OK:
            if self._exception:

                # wrapper raised most likely...
                raise self._exception
        PyBuffer_Release(buf)
        return (<int>err)

    # needed for debugging and obtaining C Attributes
    @property
    def length(self):
        return self._cparser.length

    @property
    def stream_id(self):
        return self._cparser.stream_id
    
    @property
    def type(self):
        return self._cparser.type

    @property
    def flags(self):
        return self._cparser.flags
    
    # more pytest debuggers
    @property
    def _sub_length(self):
        return self._cparser._sub_length

    @property
    def pad_length(self):
        return self._cparser.pad_length
    
    @property
    def stream_weight(self):
        return self._cparser.stream_weight
    
    @property
    def dependency_id(self):
        return self._cparser.dependency_id
    
    @property
    def error_code(self):
        return self._cparser.error_code

    @property
    def settings_id(self):
        return self._cparser.settings_id
    
    @property
    def settings_value(self):
        return self._cparser.settings_value
    
    @property
    def promise_stream_id(self):
        return self._cparser.promise_stream_id
    
    @property
    def last_stream_id(self):
        return self._cparser.last_stream_id

    @property
    def window_increment(self):
        return self._cparser.window_increment

    @property
    def altsvc_origin_length(self):
        return self._cparser.altsvc_origin_length

    @property
    def is_exclusive(self):
        return self._cparser.is_exclusive

    def __dealloc__(self):
        PyMem_Free(self._cparser)
        PyMem_Free(self._csettings)


# cdef int __span(llh2_t *s, const char *p, size_t length):
#     cdef Parser parser = <Parser>s.data
#     return parser.__span(PyBytes_FromStringAndSize(p, <Py_ssize_t>length))


cdef int cb_on_reset(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_reset()


cdef int cb_on_frame_start(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_frame_start()


cdef int cb_on_frame_end(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_frame_end()


cdef int cb_on_length_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_length_complete()


cdef int cb_on_stream_id_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_stream_id_complete()


cdef int cb_on_type_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_type_complete()


cdef int cb_on_flags_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_flags_complete()


cdef int cb_on_body(llh2_t *s, const char* p, size_t len) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_body(PyBytes_FromStringAndSize(p, <Py_ssize_t>len))


cdef int cb_on_body_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_body_complete()


cdef int cb_on_pad_length_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_pad_length_complete()


cdef int cb_on_stream_weight_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_stream_weight_complete()


cdef int cb_on_dependency_id_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_dependency_id_complete()


cdef int cb_on_error_code_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_error_code_complete()


cdef int cb_on_settings_id(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_settings_id()


cdef int cb_on_settings_value_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_settings_value_complete()


cdef int cb_on_promise_stream_id_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_promise_stream_id_complete()


cdef int cb_on_last_stream_id_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_last_stream_id_complete()


cdef int cb_on_window_increment_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_window_increment_complete()


cdef int cb_on_altsvc_origin_length_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._on_altsvc_origin_length_complete()


cdef int cb_altsvc_origin_value(llh2_t *s, const char* p, size_t len) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._altsvc_origin_value(PyBytes_FromStringAndSize(p, <Py_ssize_t>len))


cdef int cb_altsvc_origin_value_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._altsvc_origin_value_complete()


cdef int cb_altsvc_field_value(llh2_t *s, const char* p, size_t len) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._altsvc_field_value(PyBytes_FromStringAndSize(p, <Py_ssize_t>len))


cdef int cb_altsvc_field_value_complete(llh2_t *s) noexcept:
    cdef Parser parser = <Parser>s.data
    return parser._altsvc_field_value_complete()

