#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __SSE4_2__
 #ifdef _MSC_VER
  #include <nmmintrin.h>
 #else  /* !_MSC_VER */
  #include <x86intrin.h>
 #endif  /* _MSC_VER */
#endif  /* __SSE4_2__ */

#ifdef __ARM_NEON__
 #include <arm_neon.h>
#endif  /* __ARM_NEON__ */

#ifdef __wasm__
 #include <wasm_simd128.h>
#endif  /* __wasm__ */

#ifdef _MSC_VER
 #define ALIGN(n) _declspec(align(n))
 #define UNREACHABLE __assume(0)
#else  /* !_MSC_VER */
 #define ALIGN(n) __attribute__((aligned(n)))
 #define UNREACHABLE __builtin_unreachable()
#endif  /* _MSC_VER */

#include "llh2.h"

typedef int (*llh2__internal__span_cb)(
             llh2__internal_t*, const char*, const char*);

enum llparse_state_e {
  s_error,
  s_n_llh2__internal__n_pause,
  s_n_llh2__internal__n_pause_1,
  s_n_llh2__internal__n_pause_2,
  s_n_llh2__internal__n_pause_3,
  s_n_llh2__internal__n_pause_8,
  s_n_llh2__internal__n_pause_4,
  s_n_llh2__internal__n_pause_5,
  s_n_llh2__internal__n_pause_6,
  s_n_llh2__internal__n_exit,
  s_n_llh2__internal__n_pause_7,
  s_n_llh2__internal__n_error_7,
  s_n_llh2__internal__n_invoke_llh2__on_reset,
  s_n_llh2__internal__n_error_6,
  s_n_llh2__internal__n_invoke_llh2__on_frame_end,
  s_n_llh2__internal__n_error_5,
  s_n_llh2__internal__n_invoke_llh2__on_body_end,
  s_n_llh2__internal__n_span_end_llh2__on_body,
  s_n_llh2__internal__n_consume_length,
  s_n_llh2__internal__n_span_start_llh2__on_body,
  s_n_llh2__internal__n_error_4,
  s_n_llh2__internal__n_invoke_llh2__on_body_start,
  s_n_llh2__internal__n_error_9,
  s_n_llh2__internal__n_invoke_llh2__subtract_push_promise_length,
  s_n_llh2__internal__n_error_8,
  s_n_llh2__internal__n_invoke_llh2__on_promise_stream_id,
  s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte4,
  s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte3,
  s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte2,
  s_n_llh2__internal__n_promise_stream_id_uint_32_le,
  s_n_llh2__internal__n_pad_length_uint_8,
  s_n_llh2__internal__n_invoke_test_flags,
  s_n_llh2__internal__n_pause_9,
  s_n_llh2__internal__n_pause_10,
  s_n_llh2__internal__n_invoke_llh2__settings_subtract,
  s_n_llh2__internal__n_error_11,
  s_n_llh2__internal__n_invoke_llh2__on_setting_value,
  s_n_llh2__internal__n_setting_value_uint_32_le_byte4,
  s_n_llh2__internal__n_setting_value_uint_32_le_byte3,
  s_n_llh2__internal__n_setting_value_uint_32_le_byte2,
  s_n_llh2__internal__n_setting_value_uint_32_le,
  s_n_llh2__internal__n_error_10,
  s_n_llh2__internal__n_invoke_llh2__on_setting_identifier,
  s_n_llh2__internal__n_invoke_store_setting_ident,
  s_n_llh2__internal__n_error_12,
  s_n_llh2__internal__n_settings_ident_1,
  s_n_llh2__internal__n_settings_ident,
  s_n_llh2__internal__n_invoke_load_flags,
  s_n_llh2__internal__n_invoke_load_type,
  s_n_llh2__internal__n_stream_id_uint_32_le_byte4,
  s_n_llh2__internal__n_stream_id_uint_32_le_byte3,
  s_n_llh2__internal__n_stream_id_uint_32_le_byte2,
  s_n_llh2__internal__n_stream_id_uint_32_le,
  s_n_llh2__internal__n_error_3,
  s_n_llh2__internal__n_invoke_llh2__on_flags,
  s_n_llh2__internal__n_flags_uint_8,
  s_n_llh2__internal__n_error_2,
  s_n_llh2__internal__n_invoke_llh2__on_type,
  s_n_llh2__internal__n_type_uint_8,
  s_n_llh2__internal__n_error_1,
  s_n_llh2__internal__n_invoke_llh2__on_length,
  s_n_llh2__internal__n_length_uint_24_le_byte3,
  s_n_llh2__internal__n_length_uint_24_le_byte2,
  s_n_llh2__internal__n_length_uint_24_le,
  s_n_llh2__internal__n_error,
  s_n_llh2__internal__n_invoke_llh2__on_frame_start,
};
typedef enum llparse_state_e llparse_state_t;

int llh2__on_body (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__on_frame_start (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__on_length (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__on_type (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__on_flags (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__c_load_type (
  llh2__internal_t* state,
  const unsigned char* p,
    const unsigned char* endp) {
  return state->type;
}

int llh2__internal__c_test_flags (
  llh2__internal_t* state,
  const unsigned char* p,
    const unsigned char* endp) {
  return (state->flags & 8) == 8;
}

int llh2__on_promise_stream_id (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__subtract_push_promise_length (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__on_body_start (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__on_body_end (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__on_frame_end (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__on_reset (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__c_load_flags (
  llh2__internal_t* state,
  const unsigned char* p,
    const unsigned char* endp) {
  return state->flags;
}

int llh2__internal__c_store_setting_ident (
  llh2__internal_t* state,
  const unsigned char* p,
    const unsigned char* endp,
    int match) {
  state->setting_ident = match;
  return 0;
}

int llh2__on_setting_identifier (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__on_setting_value (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__settings_subtract (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal_init(llh2__internal_t* state) {
  memset(state, 0, sizeof(*state));
  state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__on_frame_start;
  return 0;
}

static llparse_state_t llh2__internal__run(
    llh2__internal_t* state,
    const unsigned char* p,
    const unsigned char* endp) {
  int match;
  switch ((llparse_state_t) (intptr_t) state->_current) {
    case s_n_llh2__internal__n_exit:
    s_n_llh2__internal__n_exit : {
      if (p == endp) {
        return s_n_llh2__internal__n_exit;
      }
      p++;
      goto s_n_llh2__internal__n_exit;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__on_reset:
    s_n_llh2__internal__n_invoke_llh2__on_reset : {
      switch (llh2__on_reset(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_exit;
        case 0:
          goto s_n_llh2__internal__n_pause_7;
        default:
          goto s_n_llh2__internal__n_error_7;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__on_frame_end:
    s_n_llh2__internal__n_invoke_llh2__on_frame_end : {
      switch (llh2__on_frame_end(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause_6;
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__on_reset;
        default:
          goto s_n_llh2__internal__n_error_6;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__on_body_end:
    s_n_llh2__internal__n_invoke_llh2__on_body_end : {
      switch (llh2__on_body_end(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause_5;
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__on_frame_end;
        default:
          goto s_n_llh2__internal__n_error_5;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_consume_length:
    s_n_llh2__internal__n_consume_length : {
      size_t avail;
      size_t need;
      
      avail = endp - p;
      need = state->length;
      if (avail >= need) {
        p += need;
        state->length = 0;
        goto s_n_llh2__internal__n_span_end_llh2__on_body;
      }
      
      state->length -= avail;
      return s_n_llh2__internal__n_consume_length;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_span_start_llh2__on_body:
    s_n_llh2__internal__n_span_start_llh2__on_body : {
      if (p == endp) {
        return s_n_llh2__internal__n_span_start_llh2__on_body;
      }
      state->_span_pos0 = (void*) p;
      goto s_n_llh2__internal__n_consume_length;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__subtract_push_promise_length:
    s_n_llh2__internal__n_invoke_llh2__subtract_push_promise_length : {
      switch (llh2__subtract_push_promise_length(state, p, endp)) {
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__on_body_start;
        default:
          goto s_n_llh2__internal__n_error_9;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__settings_subtract:
    s_n_llh2__internal__n_invoke_llh2__settings_subtract : {
      switch (llh2__settings_subtract(state, p, endp)) {
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__on_frame_end;
        default:
          goto s_n_llh2__internal__n_settings_ident;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_setting_value_uint_32_le:
    s_n_llh2__internal__n_setting_value_uint_32_le : {
      if (p == endp) {
        return s_n_llh2__internal__n_setting_value_uint_32_le;
      }
      state->setting_value = (*p);
      p++;
      goto s_n_llh2__internal__n_setting_value_uint_32_le_byte2;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_settings_ident_1:
    s_n_llh2__internal__n_settings_ident_1 : {
      if (p == endp) {
        return s_n_llh2__internal__n_settings_ident_1;
      }
      switch (*p){
        case 1: {
          p++;
          match = 1;
          goto s_n_llh2__internal__n_invoke_store_setting_ident;
        }
        case 2: {
          p++;
          match = 2;
          goto s_n_llh2__internal__n_invoke_store_setting_ident;
        }
        case 3: {
          p++;
          match = 3;
          goto s_n_llh2__internal__n_invoke_store_setting_ident;
        }
        case 4: {
          p++;
          match = 4;
          goto s_n_llh2__internal__n_invoke_store_setting_ident;
        }
        case 5: {
          p++;
          match = 5;
          goto s_n_llh2__internal__n_invoke_store_setting_ident;
        }
        case 6: {
          p++;
          match = 6;
          goto s_n_llh2__internal__n_invoke_store_setting_ident;
        }
        case 8: {
          p++;
          match = 8;
          goto s_n_llh2__internal__n_invoke_store_setting_ident;
        }
        default: {
          goto s_n_llh2__internal__n_error_12;
        }
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_settings_ident:
    s_n_llh2__internal__n_settings_ident : {
      if (p == endp) {
        return s_n_llh2__internal__n_settings_ident;
      }
      switch (*p){
        case 0: {
          p++;
          goto s_n_llh2__internal__n_settings_ident_1;
        }
        default: {
          goto s_n_llh2__internal__n_error_12;
        }
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_stream_id_uint_32_le:
    s_n_llh2__internal__n_stream_id_uint_32_le : {
      if (p == endp) {
        return s_n_llh2__internal__n_stream_id_uint_32_le;
      }
      state->stream_id = (*p);
      p++;
      goto s_n_llh2__internal__n_stream_id_uint_32_le_byte2;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_flags_uint_8:
    s_n_llh2__internal__n_flags_uint_8 : {
      if (p == endp) {
        return s_n_llh2__internal__n_flags_uint_8;
      }
      state->flags = (*p);
      p++;
      goto s_n_llh2__internal__n_invoke_llh2__on_flags;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_type_uint_8:
    s_n_llh2__internal__n_type_uint_8 : {
      if (p == endp) {
        return s_n_llh2__internal__n_type_uint_8;
      }
      state->type = (*p);
      p++;
      goto s_n_llh2__internal__n_invoke_llh2__on_type;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_length_uint_24_le:
    s_n_llh2__internal__n_length_uint_24_le : {
      if (p == endp) {
        return s_n_llh2__internal__n_length_uint_24_le;
      }
      state->length = (*p);
      p++;
      goto s_n_llh2__internal__n_length_uint_24_le_byte2;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__on_frame_start:
    s_n_llh2__internal__n_invoke_llh2__on_frame_start : {
      switch (llh2__on_frame_start(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause;
        case 0:
          goto s_n_llh2__internal__n_length_uint_24_le;
        default:
          goto s_n_llh2__internal__n_error;
      }
      UNREACHABLE;
    }
    default:
      UNREACHABLE;
  }
  s_n_llh2__internal__n_pause: {
    state->error = 0x1;
    state->reason = "on_frame_start pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_length_uint_24_le;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_1: {
    state->error = 0x1;
    state->reason = "on_length pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_type_uint_8;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_2: {
    state->error = 0x1;
    state->reason = "on_type pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_flags_uint_8;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_3: {
    state->error = 0x1;
    state->reason = "on_flags pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_stream_id_uint_32_le;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_8: {
    state->error = 0x1;
    state->reason = "on_promise_stream_id pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__subtract_push_promise_length;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_4: {
    state->error = 0x1;
    state->reason = "on_body_start pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_span_start_llh2__on_body;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_5: {
    state->error = 0x1;
    state->reason = "on_body_end pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_6: {
    state->error = 0x1;
    state->reason = "on_frame_end pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__on_reset;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_7: {
    state->error = 0x1;
    state->reason = "paused parser at on_reset";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__on_frame_start;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_7: {
    state->error = 0xa;
    state->reason = "on_reset callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_6: {
    state->error = 0x3;
    state->reason = "'on_frame_end' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_5: {
    state->error = 0x9;
    state->reason = "'on_body_end' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_end_llh2__on_body: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = llh2__on_body(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__on_body_end;
      return s_error;
    }
    goto s_n_llh2__internal__n_invoke_llh2__on_body_end;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_4: {
    state->error = 0x8;
    state->reason = "'on_body_start' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__on_body_start: {
    switch (llh2__on_body_start(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_4;
      case 0:
        goto s_n_llh2__internal__n_span_start_llh2__on_body;
      default:
        goto s_n_llh2__internal__n_error_4;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_9: {
    state->error = 0x10;
    state->reason = "invalid pad length provided";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_8: {
    state->error = 0xd;
    state->reason = "'on_promise_stream_id' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__on_promise_stream_id: {
    switch (llh2__on_promise_stream_id(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_8;
      case 0:
        goto s_n_llh2__internal__n_invoke_llh2__subtract_push_promise_length;
      default:
        goto s_n_llh2__internal__n_error_8;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte4: {
    if (p == endp) {
      return s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte4;
    }
    state->promise_stream_id = (state->promise_stream_id >> 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_invoke_llh2__on_promise_stream_id;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte3: {
    if (p == endp) {
      return s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte3;
    }
    state->promise_stream_id = (state->promise_stream_id >> 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte4;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte2: {
    if (p == endp) {
      return s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte2;
    }
    state->promise_stream_id = (state->promise_stream_id >> 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte3;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_promise_stream_id_uint_32_le: {
    if (p == endp) {
      return s_n_llh2__internal__n_promise_stream_id_uint_32_le;
    }
    state->promise_stream_id = (*p);
    p++;
    goto s_n_llh2__internal__n_promise_stream_id_uint_32_le_byte2;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pad_length_uint_8: {
    if (p == endp) {
      return s_n_llh2__internal__n_pad_length_uint_8;
    }
    state->pad_length = (*p);
    p++;
    goto s_n_llh2__internal__n_promise_stream_id_uint_32_le;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_test_flags: {
    switch (llh2__internal__c_test_flags(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pad_length_uint_8;
      default:
        goto s_n_llh2__internal__n_promise_stream_id_uint_32_le;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_9: {
    state->error = 0x1;
    state->reason = "on_setting_identifier pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_setting_value_uint_32_le;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_10: {
    state->error = 0x1;
    state->reason = "on_setting_value pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__settings_subtract;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_11: {
    state->error = 0xc;
    state->reason = "'on_setting_value' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__on_setting_value: {
    switch (llh2__on_setting_value(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_10;
      case 0:
        goto s_n_llh2__internal__n_invoke_llh2__settings_subtract;
      default:
        goto s_n_llh2__internal__n_error_11;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_setting_value_uint_32_le_byte4: {
    if (p == endp) {
      return s_n_llh2__internal__n_setting_value_uint_32_le_byte4;
    }
    state->setting_value = (state->setting_value >> 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_invoke_llh2__on_setting_value;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_setting_value_uint_32_le_byte3: {
    if (p == endp) {
      return s_n_llh2__internal__n_setting_value_uint_32_le_byte3;
    }
    state->setting_value = (state->setting_value >> 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_setting_value_uint_32_le_byte4;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_setting_value_uint_32_le_byte2: {
    if (p == endp) {
      return s_n_llh2__internal__n_setting_value_uint_32_le_byte2;
    }
    state->setting_value = (state->setting_value >> 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_setting_value_uint_32_le_byte3;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_10: {
    state->error = 0xb;
    state->reason = "'on_setting_identifier' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__on_setting_identifier: {
    switch (llh2__on_setting_identifier(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_9;
      case 0:
        goto s_n_llh2__internal__n_setting_value_uint_32_le;
      default:
        goto s_n_llh2__internal__n_error_10;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_store_setting_ident: {
    switch (llh2__internal__c_store_setting_ident(state, p, endp, match)) {
      default:
        goto s_n_llh2__internal__n_invoke_llh2__on_setting_identifier;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_12: {
    state->error = 0xe;
    state->reason = "settings identifier is invalid";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_load_flags: {
    switch (llh2__internal__c_load_flags(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_invoke_llh2__on_frame_end;
      default:
        goto s_n_llh2__internal__n_settings_ident;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_load_type: {
    switch (llh2__internal__c_load_type(state, p, endp)) {
      case 5:
        goto s_n_llh2__internal__n_invoke_test_flags;
      case 4:
        goto s_n_llh2__internal__n_invoke_load_flags;
      default:
        goto s_n_llh2__internal__n_invoke_llh2__on_body_start;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_stream_id_uint_32_le_byte4: {
    if (p == endp) {
      return s_n_llh2__internal__n_stream_id_uint_32_le_byte4;
    }
    state->stream_id = (state->stream_id >> 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_invoke_load_type;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_stream_id_uint_32_le_byte3: {
    if (p == endp) {
      return s_n_llh2__internal__n_stream_id_uint_32_le_byte3;
    }
    state->stream_id = (state->stream_id >> 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_stream_id_uint_32_le_byte4;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_stream_id_uint_32_le_byte2: {
    if (p == endp) {
      return s_n_llh2__internal__n_stream_id_uint_32_le_byte2;
    }
    state->stream_id = (state->stream_id >> 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_stream_id_uint_32_le_byte3;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_3: {
    state->error = 0x7;
    state->reason = "'on_flags' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__on_flags: {
    switch (llh2__on_flags(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_3;
      case 0:
        goto s_n_llh2__internal__n_stream_id_uint_32_le;
      default:
        goto s_n_llh2__internal__n_error_3;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_2: {
    state->error = 0x6;
    state->reason = "'on_type' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__on_type: {
    switch (llh2__on_type(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_2;
      case 0:
        goto s_n_llh2__internal__n_flags_uint_8;
      default:
        goto s_n_llh2__internal__n_error_2;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_1: {
    state->error = 0x4;
    state->reason = "'on_length' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__on_length: {
    switch (llh2__on_length(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_1;
      case 0:
        goto s_n_llh2__internal__n_type_uint_8;
      default:
        goto s_n_llh2__internal__n_error_1;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_length_uint_24_le_byte3: {
    if (p == endp) {
      return s_n_llh2__internal__n_length_uint_24_le_byte3;
    }
    state->length = (state->length >> 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_invoke_llh2__on_length;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_length_uint_24_le_byte2: {
    if (p == endp) {
      return s_n_llh2__internal__n_length_uint_24_le_byte2;
    }
    state->length = (state->length >> 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_length_uint_24_le_byte3;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error: {
    state->error = 0x2;
    state->reason = "'on_frame_start' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
}

int llh2__internal_execute(llh2__internal_t* state, const char* p, const char* endp) {
  llparse_state_t next;

  /* check lingering errors */
  if (state->error != 0) {
    return state->error;
  }

  /* restart spans */
  if (state->_span_pos0 != NULL) {
    state->_span_pos0 = (void*) p;
  }
  
  next = llh2__internal__run(state, (const unsigned char*) p, (const unsigned char*) endp);
  if (next == s_error) {
    return state->error;
  }
  state->_current = (void*) (intptr_t) next;

  /* execute spans */
  if (state->_span_pos0 != NULL) {
    int error;
  
    error = llh2__on_body(state, state->_span_pos0, (const char*) endp);
    if (error != 0) {
      state->error = error;
      state->error_pos = endp;
      return error;
    }
  }
  
  return 0;
}