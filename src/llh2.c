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
  s_n_llh2__internal__n_pause_4,
  s_n_llh2__internal__n_error_11,
  s_n_llh2__internal__n_pause_8,
  s_n_llh2__internal__n_pause_9,
  s_n_llh2__internal__n_pause_10,
  s_n_llh2__internal__n_pause_6,
  s_n_llh2__internal__n_exit,
  s_n_llh2__internal__n_pause_7,
  s_n_llh2__internal__n_error_7,
  s_n_llh2__internal__n_invoke_llh2__internal__on_reset,
  s_n_llh2__internal__n_error_6,
  s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end,
  s_n_llh2__internal__n_error_10,
  s_n_llh2__internal__n_invoke_llh2__internal__altsvc_field_value_complete,
  s_n_llh2__internal__n_span_end_llh2__internal__altsvc_field_value,
  s_n_llh2__internal__n_consume__sub_length,
  s_n_llh2__internal__n_span_start_llh2__internal__altsvc_field_value,
  s_n_llh2__internal__n_error_9,
  s_n_llh2__internal__n_invoke_llh2__internal__altsvc_origin_value_complete,
  s_n_llh2__internal__n_span_end_llh2__internal__altsvc_origin_value,
  s_n_llh2__internal__n_consume_altsvc_origin_length,
  s_n_llh2__internal__n_span_start_llh2__internal__altsvc_origin_value,
  s_n_llh2__internal__n_error_8,
  s_n_llh2__internal__n_invoke_llh2__internal__on_altsvc_origin_length_complete,
  s_n_llh2__internal__n_invoke_llh2__before_origin_length_complete,
  s_n_llh2__internal__n_pause_11,
  s_n_llh2__internal__n_error_12,
  s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_1,
  s_n_llh2__internal__n_span_end_llh2__internal__on_body_1,
  s_n_llh2__internal__n_consume_length_1,
  s_n_llh2__internal__n_span_start_llh2__internal__on_body_1,
  s_n_llh2__internal__n_error_14,
  s_n_llh2__internal__n_pause_12,
  s_n_llh2__internal__n_error_13,
  s_n_llh2__internal__n_invoke_llh2__internal__on_window_increment_complete,
  s_n_llh2__internal__n_window_increment_uint_32be_byte4,
  s_n_llh2__internal__n_window_increment_uint_32be_byte3,
  s_n_llh2__internal__n_window_increment_uint_32be_byte2,
  s_n_llh2__internal__n_window_increment_uint_32be,
  s_n_llh2__internal__n_invoke_is_equal_length,
  s_n_llh2__internal__n_error_16,
  s_n_llh2__internal__n_pause_13,
  s_n_llh2__internal__n_error_15,
  s_n_llh2__internal__n_invoke_llh2__internal__on_last_stream_id_complete,
  s_n_llh2__internal__n_invoke_llh2__check_goaway_frame_length,
  s_n_llh2__internal__n_error_18,
  s_n_llh2__internal__n_pause_14,
  s_n_llh2__internal__n_error_17,
  s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_2,
  s_n_llh2__internal__n_span_end_llh2__internal__on_body_2,
  s_n_llh2__internal__n_consume_length_2,
  s_n_llh2__internal__n_span_start_llh2__internal__on_body_2,
  s_n_llh2__internal__n_invoke_is_equal_length_1,
  s_n_llh2__internal__n_error_21,
  s_n_llh2__internal__n_pause_16,
  s_n_llh2__internal__n_error_20,
  s_n_llh2__internal__n_invoke_llh2__internal__on_promise_stream_id_complete,
  s_n_llh2__internal__n_invoke_llh2__before_promise_stream_id_complete,
  s_n_llh2__internal__n_error_22,
  s_n_llh2__internal__n_pause_15,
  s_n_llh2__internal__n_consume_pad_length,
  s_n_llh2__internal__n_error_19,
  s_n_llh2__internal__n_invoke_llh2__internal__on_pad_length_complete,
  s_n_llh2__internal__n_pad_length_uint_8,
  s_n_llh2__internal__n_invoke_llh2__before_pad_length,
  s_n_llh2__internal__n_invoke_test_flags,
  s_n_llh2__internal__n_pause_17,
  s_n_llh2__internal__n_pause_18,
  s_n_llh2__internal__n_invoke_llh2__settings_subtract,
  s_n_llh2__internal__n_error_24,
  s_n_llh2__internal__n_invoke_llh2__internal__on_settings_value_complete,
  s_n_llh2__internal__n_settings_value_uint_32be_byte4,
  s_n_llh2__internal__n_settings_value_uint_32be_byte3,
  s_n_llh2__internal__n_settings_value_uint_32be_byte2,
  s_n_llh2__internal__n_settings_value_uint_32be,
  s_n_llh2__internal__n_error_23,
  s_n_llh2__internal__n_invoke_llh2__internal__on_settings_id,
  s_n_llh2__internal__n_invoke_store_settings_id,
  s_n_llh2__internal__n_error_25,
  s_n_llh2__internal__n_ident_1,
  s_n_llh2__internal__n_ident,
  s_n_llh2__internal__n_invoke_test_flags_1,
  s_n_llh2__internal__n_error_27,
  s_n_llh2__internal__n_pause_19,
  s_n_llh2__internal__n_error_26,
  s_n_llh2__internal__n_invoke_llh2__internal__on_error_code_complete,
  s_n_llh2__internal__n_error_code_uint_32be_byte4,
  s_n_llh2__internal__n_error_code_uint_32be_byte3,
  s_n_llh2__internal__n_error_code_uint_32be_byte2,
  s_n_llh2__internal__n_error_code_uint_32be,
  s_n_llh2__internal__n_invoke_is_equal_length_2,
  s_n_llh2__internal__n_error_30,
  s_n_llh2__internal__n_pause_20,
  s_n_llh2__internal__n_pause_21,
  s_n_llh2__internal__n_error_29,
  s_n_llh2__internal__n_invoke_llh2__internal__on_stream_weight_complete,
  s_n_llh2__internal__n_stream_weight_uint_8,
  s_n_llh2__internal__n_error_28,
  s_n_llh2__internal__n_invoke_llh2__internal__on_dependency_id_complete,
  s_n_llh2__internal__n_invoke_llh2__post_dependency_id,
  s_n_llh2__internal__n_invoke_llh2__before_priority,
  s_n_llh2__internal__n_pause_25,
  s_n_llh2__internal__n_error_34,
  s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_3,
  s_n_llh2__internal__n_span_end_llh2__internal__on_body_3,
  s_n_llh2__internal__n_consume__sub_length_1,
  s_n_llh2__internal__n_span_start_llh2__internal__on_body_3,
  s_n_llh2__internal__n_error_35,
  s_n_llh2__internal__n_pause_23,
  s_n_llh2__internal__n_pause_24,
  s_n_llh2__internal__n_error_33,
  s_n_llh2__internal__n_invoke_llh2__internal__on_stream_weight_complete_1,
  s_n_llh2__internal__n_stream_weight_uint_8_1,
  s_n_llh2__internal__n_error_32,
  s_n_llh2__internal__n_invoke_llh2__internal__on_dependency_id_complete_1,
  s_n_llh2__internal__n_invoke_llh2__post_dependency_id_1,
  s_n_llh2__internal__n_invoke_llh2__before_priority_1,
  s_n_llh2__internal__n_invoke_test_flags_3,
  s_n_llh2__internal__n_error_36,
  s_n_llh2__internal__n_pause_22,
  s_n_llh2__internal__n_consume_pad_length_1,
  s_n_llh2__internal__n_error_31,
  s_n_llh2__internal__n_invoke_llh2__internal__on_pad_length_complete_1,
  s_n_llh2__internal__n_pad_length_uint_8_1,
  s_n_llh2__internal__n_invoke_llh2__before_pad_length_1,
  s_n_llh2__internal__n_invoke_test_flags_2,
  s_n_llh2__internal__n_pause_27,
  s_n_llh2__internal__n_error_38,
  s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_4,
  s_n_llh2__internal__n_span_end_llh2__internal__on_body_4,
  s_n_llh2__internal__n_consume__sub_length_2,
  s_n_llh2__internal__n_span_start_llh2__internal__on_body_4,
  s_n_llh2__internal__n_invoke_llh2__before_data_frame_body_start,
  s_n_llh2__internal__n_error_39,
  s_n_llh2__internal__n_pause_26,
  s_n_llh2__internal__n_consume_pad_length_2,
  s_n_llh2__internal__n_error_37,
  s_n_llh2__internal__n_invoke_llh2__internal__on_pad_length_complete_2,
  s_n_llh2__internal__n_pad_length_uint_8_2,
  s_n_llh2__internal__n_invoke_llh2__before_pad_length_2,
  s_n_llh2__internal__n_invoke_test_flags_4,
  s_n_llh2__internal__n_pause_5,
  s_n_llh2__internal__n_error_5,
  s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete,
  s_n_llh2__internal__n_span_end_llh2__internal__on_body,
  s_n_llh2__internal__n_consume_length,
  s_n_llh2__internal__n_span_start_llh2__internal__on_body,
  s_n_llh2__internal__n_invoke_load_type,
  s_n_llh2__internal__n_error_4,
  s_n_llh2__internal__n_invoke_llh2__internal__on_stream_id_complete,
  s_n_llh2__internal__n_stream_id_int_32be_byte4,
  s_n_llh2__internal__n_stream_id_int_32be_byte3,
  s_n_llh2__internal__n_stream_id_int_32be_byte2,
  s_n_llh2__internal__n_stream_id_int_32be,
  s_n_llh2__internal__n_error_3,
  s_n_llh2__internal__n_invoke_llh2__internal__on_flags_complete,
  s_n_llh2__internal__n_flags_uint_8,
  s_n_llh2__internal__n_error_2,
  s_n_llh2__internal__n_invoke_llh2__internal__on_type_complete,
  s_n_llh2__internal__n_type_uint_8,
  s_n_llh2__internal__n_error_1,
  s_n_llh2__internal__n_invoke_llh2__internal__on_length_complete,
  s_n_llh2__internal__n_length_uint_24be_byte3,
  s_n_llh2__internal__n_length_uint_24be_byte2,
  s_n_llh2__internal__n_length_uint_24be,
  s_n_llh2__internal__n_error,
  s_n_llh2__internal__n_invoke_llh2__internal__on_frame_start,
};
typedef enum llparse_state_e llparse_state_t;

int llh2__internal__altsvc_field_value (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__altsvc_origin_value (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_body (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_frame_start (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_length_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_type_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_flags_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_stream_id_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__c_load_type (
  llh2__internal_t* state,
  const unsigned char* p,
    const unsigned char* endp) {
  return state->type;
}

int llh2__before_origin_length_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_altsvc_origin_length_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__altsvc_origin_value_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__altsvc_field_value_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_frame_end (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_reset (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_body_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__c_is_equal_length (
  llh2__internal_t* state,
  const unsigned char* p,
    const unsigned char* endp) {
  return state->length == 4;
}

int llh2__internal__on_window_increment_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__check_goaway_frame_length (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_last_stream_id_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__c_is_equal_length_1 (
  llh2__internal_t* state,
  const unsigned char* p,
    const unsigned char* endp) {
  return state->length == 8;
}

int llh2__internal__c_test_flags (
  llh2__internal_t* state,
  const unsigned char* p,
    const unsigned char* endp) {
  return (state->flags & 8) == 8;
}

int llh2__before_promise_stream_id_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_promise_stream_id_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__before_pad_length (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_pad_length_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__c_test_flags_1 (
  llh2__internal_t* state,
  const unsigned char* p,
    const unsigned char* endp) {
  return (state->flags & 1) == 1;
}

int llh2__internal__c_store_settings_id (
  llh2__internal_t* state,
  const unsigned char* p,
    const unsigned char* endp,
    int match) {
  state->settings_id = match;
  return 0;
}

int llh2__internal__on_settings_id (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_settings_value_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__settings_subtract (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_error_code_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__before_priority (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__post_dependency_id (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_dependency_id_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__on_stream_weight_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal__c_test_flags_3 (
  llh2__internal_t* state,
  const unsigned char* p,
    const unsigned char* endp) {
  return (state->flags & 32) == 32;
}

int llh2__before_data_frame_body_start (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int llh2__internal_init(llh2__internal_t* state) {
  memset(state, 0, sizeof(*state));
  state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_start;
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
    case s_n_llh2__internal__n_invoke_llh2__internal__on_reset:
    s_n_llh2__internal__n_invoke_llh2__internal__on_reset : {
      switch (llh2__internal__on_reset(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_exit;
        case 0:
          goto s_n_llh2__internal__n_pause_7;
        default:
          goto s_n_llh2__internal__n_error_7;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end:
    s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end : {
      switch (llh2__internal__on_frame_end(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause_6;
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__internal__on_reset;
        default:
          goto s_n_llh2__internal__n_error_6;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__internal__altsvc_field_value_complete:
    s_n_llh2__internal__n_invoke_llh2__internal__altsvc_field_value_complete : {
      switch (llh2__internal__altsvc_field_value_complete(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause_10;
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
        default:
          goto s_n_llh2__internal__n_error_10;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_consume__sub_length:
    s_n_llh2__internal__n_consume__sub_length : {
      size_t avail;
      size_t need;
      
      avail = endp - p;
      need = state->_sub_length;
      if (avail >= need) {
        p += need;
        state->_sub_length = 0;
        goto s_n_llh2__internal__n_span_end_llh2__internal__altsvc_field_value;
      }
      
      state->_sub_length -= avail;
      return s_n_llh2__internal__n_consume__sub_length;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_span_start_llh2__internal__altsvc_field_value:
    s_n_llh2__internal__n_span_start_llh2__internal__altsvc_field_value : {
      if (p == endp) {
        return s_n_llh2__internal__n_span_start_llh2__internal__altsvc_field_value;
      }
      state->_span_pos0 = (void*) p;
      state->_span_cb0 = llh2__internal__altsvc_field_value;
      goto s_n_llh2__internal__n_consume__sub_length;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__internal__altsvc_origin_value_complete:
    s_n_llh2__internal__n_invoke_llh2__internal__altsvc_origin_value_complete : {
      switch (llh2__internal__altsvc_origin_value_complete(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause_9;
        case 0:
          goto s_n_llh2__internal__n_span_start_llh2__internal__altsvc_field_value;
        default:
          goto s_n_llh2__internal__n_error_9;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_consume_altsvc_origin_length:
    s_n_llh2__internal__n_consume_altsvc_origin_length : {
      size_t avail;
      size_t need;
      
      avail = endp - p;
      need = state->altsvc_origin_length;
      if (avail >= need) {
        p += need;
        state->altsvc_origin_length = 0;
        goto s_n_llh2__internal__n_span_end_llh2__internal__altsvc_origin_value;
      }
      
      state->altsvc_origin_length -= avail;
      return s_n_llh2__internal__n_consume_altsvc_origin_length;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_span_start_llh2__internal__altsvc_origin_value:
    s_n_llh2__internal__n_span_start_llh2__internal__altsvc_origin_value : {
      if (p == endp) {
        return s_n_llh2__internal__n_span_start_llh2__internal__altsvc_origin_value;
      }
      state->_span_pos0 = (void*) p;
      state->_span_cb0 = llh2__internal__altsvc_origin_value;
      goto s_n_llh2__internal__n_consume_altsvc_origin_length;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_1:
    s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_1 : {
      switch (llh2__internal__on_body_complete(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause_11;
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
        default:
          goto s_n_llh2__internal__n_error_12;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_consume_length_1:
    s_n_llh2__internal__n_consume_length_1 : {
      size_t avail;
      size_t need;
      
      avail = endp - p;
      need = state->length;
      if (avail >= need) {
        p += need;
        state->length = 0;
        goto s_n_llh2__internal__n_span_end_llh2__internal__on_body_1;
      }
      
      state->length -= avail;
      return s_n_llh2__internal__n_consume_length_1;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_2:
    s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_2 : {
      switch (llh2__internal__on_body_complete(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause_14;
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
        default:
          goto s_n_llh2__internal__n_error_17;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_consume_length_2:
    s_n_llh2__internal__n_consume_length_2 : {
      size_t avail;
      size_t need;
      
      avail = endp - p;
      need = state->length;
      if (avail >= need) {
        p += need;
        state->length = 0;
        goto s_n_llh2__internal__n_span_end_llh2__internal__on_body_2;
      }
      
      state->length -= avail;
      return s_n_llh2__internal__n_consume_length_2;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_consume_pad_length:
    s_n_llh2__internal__n_consume_pad_length : {
      size_t avail;
      size_t need;
      
      avail = endp - p;
      need = state->pad_length;
      if (avail >= need) {
        p += need;
        state->pad_length = 0;
        p++;
        goto s_n_llh2__internal__n_invoke_llh2__before_promise_stream_id_complete;
      }
      
      state->pad_length -= avail;
      return s_n_llh2__internal__n_consume_pad_length;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__settings_subtract:
    s_n_llh2__internal__n_invoke_llh2__settings_subtract : {
      switch (llh2__settings_subtract(state, p, endp)) {
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
        default:
          goto s_n_llh2__internal__n_ident;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_settings_value_uint_32be:
    s_n_llh2__internal__n_settings_value_uint_32be : {
      if (p == endp) {
        return s_n_llh2__internal__n_settings_value_uint_32be;
      }
      state->settings_value = (*p);
      p++;
      goto s_n_llh2__internal__n_settings_value_uint_32be_byte2;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_ident_1:
    s_n_llh2__internal__n_ident_1 : {
      if (p == endp) {
        return s_n_llh2__internal__n_ident_1;
      }
      switch (*p){
        case 1: {
          p++;
          match = 1;
          goto s_n_llh2__internal__n_invoke_store_settings_id;
        }
        case 2: {
          p++;
          match = 2;
          goto s_n_llh2__internal__n_invoke_store_settings_id;
        }
        case 3: {
          p++;
          match = 3;
          goto s_n_llh2__internal__n_invoke_store_settings_id;
        }
        case 4: {
          p++;
          match = 4;
          goto s_n_llh2__internal__n_invoke_store_settings_id;
        }
        case 5: {
          p++;
          match = 5;
          goto s_n_llh2__internal__n_invoke_store_settings_id;
        }
        case 6: {
          p++;
          match = 6;
          goto s_n_llh2__internal__n_invoke_store_settings_id;
        }
        case 8: {
          p++;
          match = 8;
          goto s_n_llh2__internal__n_invoke_store_settings_id;
        }
        default: {
          goto s_n_llh2__internal__n_error_25;
        }
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_ident:
    s_n_llh2__internal__n_ident : {
      if (p == endp) {
        return s_n_llh2__internal__n_ident;
      }
      switch (*p){
        case 0: {
          p++;
          goto s_n_llh2__internal__n_ident_1;
        }
        default: {
          goto s_n_llh2__internal__n_error_25;
        }
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_stream_weight_uint_8:
    s_n_llh2__internal__n_stream_weight_uint_8 : {
      if (p == endp) {
        return s_n_llh2__internal__n_stream_weight_uint_8;
      }
      state->stream_weight = (*p);
      p++;
      goto s_n_llh2__internal__n_invoke_llh2__internal__on_stream_weight_complete;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_3:
    s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_3 : {
      switch (llh2__internal__on_body_complete(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause_25;
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
        default:
          goto s_n_llh2__internal__n_error_34;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_consume__sub_length_1:
    s_n_llh2__internal__n_consume__sub_length_1 : {
      size_t avail;
      size_t need;
      
      avail = endp - p;
      need = state->_sub_length;
      if (avail >= need) {
        p += need;
        state->_sub_length = 0;
        goto s_n_llh2__internal__n_span_end_llh2__internal__on_body_3;
      }
      
      state->_sub_length -= avail;
      return s_n_llh2__internal__n_consume__sub_length_1;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_span_start_llh2__internal__on_body_3:
    s_n_llh2__internal__n_span_start_llh2__internal__on_body_3 : {
      if (p == endp) {
        return s_n_llh2__internal__n_span_start_llh2__internal__on_body_3;
      }
      state->_span_pos0 = (void*) p;
      state->_span_cb0 = llh2__internal__on_body;
      goto s_n_llh2__internal__n_consume__sub_length_1;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_stream_weight_uint_8_1:
    s_n_llh2__internal__n_stream_weight_uint_8_1 : {
      if (p == endp) {
        return s_n_llh2__internal__n_stream_weight_uint_8_1;
      }
      state->stream_weight = (*p);
      p++;
      goto s_n_llh2__internal__n_invoke_llh2__internal__on_stream_weight_complete_1;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_consume_pad_length_1:
    s_n_llh2__internal__n_consume_pad_length_1 : {
      size_t avail;
      size_t need;
      
      avail = endp - p;
      need = state->pad_length;
      if (avail >= need) {
        p += need;
        state->pad_length = 0;
        p++;
        goto s_n_llh2__internal__n_invoke_test_flags_3;
      }
      
      state->pad_length -= avail;
      return s_n_llh2__internal__n_consume_pad_length_1;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_4:
    s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_4 : {
      switch (llh2__internal__on_body_complete(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause_27;
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
        default:
          goto s_n_llh2__internal__n_error_38;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_consume__sub_length_2:
    s_n_llh2__internal__n_consume__sub_length_2 : {
      size_t avail;
      size_t need;
      
      avail = endp - p;
      need = state->_sub_length;
      if (avail >= need) {
        p += need;
        state->_sub_length = 0;
        goto s_n_llh2__internal__n_span_end_llh2__internal__on_body_4;
      }
      
      state->_sub_length -= avail;
      return s_n_llh2__internal__n_consume__sub_length_2;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_consume_pad_length_2:
    s_n_llh2__internal__n_consume_pad_length_2 : {
      size_t avail;
      size_t need;
      
      avail = endp - p;
      need = state->pad_length;
      if (avail >= need) {
        p += need;
        state->pad_length = 0;
        p++;
        goto s_n_llh2__internal__n_invoke_llh2__before_data_frame_body_start;
      }
      
      state->pad_length -= avail;
      return s_n_llh2__internal__n_consume_pad_length_2;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete:
    s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete : {
      switch (llh2__internal__on_body_complete(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause_5;
        case 0:
          goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
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
        goto s_n_llh2__internal__n_span_end_llh2__internal__on_body;
      }
      
      state->length -= avail;
      return s_n_llh2__internal__n_consume_length;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_load_type:
    s_n_llh2__internal__n_invoke_load_type : {
      switch (llh2__internal__c_load_type(state, p, endp)) {
        case 10:
          goto s_n_llh2__internal__n_invoke_llh2__before_origin_length_complete;
        case 9:
          goto s_n_llh2__internal__n_span_start_llh2__internal__on_body_1;
        case 8:
          goto s_n_llh2__internal__n_invoke_is_equal_length;
        case 7:
          goto s_n_llh2__internal__n_invoke_llh2__check_goaway_frame_length;
        case 6:
          goto s_n_llh2__internal__n_invoke_is_equal_length_1;
        case 5:
          goto s_n_llh2__internal__n_invoke_test_flags;
        case 4:
          goto s_n_llh2__internal__n_invoke_test_flags_1;
        case 3:
          goto s_n_llh2__internal__n_invoke_is_equal_length_2;
        case 2:
          goto s_n_llh2__internal__n_invoke_llh2__before_priority;
        case 1:
          goto s_n_llh2__internal__n_invoke_test_flags_2;
        case 0:
          goto s_n_llh2__internal__n_invoke_test_flags_4;
        default:
          goto s_n_llh2__internal__n_span_start_llh2__internal__on_body;
      }
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_stream_id_int_32be:
    s_n_llh2__internal__n_stream_id_int_32be : {
      if (p == endp) {
        return s_n_llh2__internal__n_stream_id_int_32be;
      }
      state->stream_id = (*p);
      p++;
      goto s_n_llh2__internal__n_stream_id_int_32be_byte2;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_flags_uint_8:
    s_n_llh2__internal__n_flags_uint_8 : {
      if (p == endp) {
        return s_n_llh2__internal__n_flags_uint_8;
      }
      state->flags = (*p);
      p++;
      goto s_n_llh2__internal__n_invoke_llh2__internal__on_flags_complete;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_type_uint_8:
    s_n_llh2__internal__n_type_uint_8 : {
      if (p == endp) {
        return s_n_llh2__internal__n_type_uint_8;
      }
      state->type = (*p);
      p++;
      goto s_n_llh2__internal__n_invoke_llh2__internal__on_type_complete;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_length_uint_24be:
    s_n_llh2__internal__n_length_uint_24be : {
      if (p == endp) {
        return s_n_llh2__internal__n_length_uint_24be;
      }
      state->length = (*p);
      p++;
      goto s_n_llh2__internal__n_length_uint_24be_byte2;
      UNREACHABLE;
    }
    case s_n_llh2__internal__n_invoke_llh2__internal__on_frame_start:
    s_n_llh2__internal__n_invoke_llh2__internal__on_frame_start : {
      switch (llh2__internal__on_frame_start(state, p, endp)) {
        case 1:
          goto s_n_llh2__internal__n_pause;
        case 0:
          goto s_n_llh2__internal__n_length_uint_24be;
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
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_length_uint_24be;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_1: {
    state->error = 0x1;
    state->reason = "on_length_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_type_uint_8;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_2: {
    state->error = 0x1;
    state->reason = "on_type_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_flags_uint_8;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_3: {
    state->error = 0x1;
    state->reason = "on_flags_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_stream_id_int_32be;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_4: {
    state->error = 0x1;
    state->reason = "on_stream_id_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_load_type;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_11: {
    state->error = 0x18;
    state->reason = "length conflicts with ALTSVC origin length";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_8: {
    state->error = 0x1;
    state->reason = "on_altsvc_origin_length_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_span_start_llh2__internal__altsvc_origin_value;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_9: {
    state->error = 0x1;
    state->reason = "altsvc_origin_value_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_span_start_llh2__internal__altsvc_field_value;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_10: {
    state->error = 0x1;
    state->reason = "altsvc_field_value_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_6: {
    state->error = 0x1;
    state->reason = "on_frame_end pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_reset;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_7: {
    state->error = 0x1;
    state->reason = "paused parser at on_reset";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_start;
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
  s_n_llh2__internal__n_error_10: {
    state->error = 0x16;
    state->reason = "'altsvc_field_value_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_end_llh2__internal__altsvc_field_value: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = llh2__internal__altsvc_field_value(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__altsvc_field_value_complete;
      return s_error;
    }
    goto s_n_llh2__internal__n_invoke_llh2__internal__altsvc_field_value_complete;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_9: {
    state->error = 0x15;
    state->reason = "'altsvc_origin_value_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_end_llh2__internal__altsvc_origin_value: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = llh2__internal__altsvc_origin_value(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__altsvc_origin_value_complete;
      return s_error;
    }
    goto s_n_llh2__internal__n_invoke_llh2__internal__altsvc_origin_value_complete;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_8: {
    state->error = 0x14;
    state->reason = "'on_altsvc_origin_length_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_altsvc_origin_length_complete: {
    switch (llh2__internal__on_altsvc_origin_length_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_8;
      case 0:
        goto s_n_llh2__internal__n_span_start_llh2__internal__altsvc_origin_value;
      default:
        goto s_n_llh2__internal__n_error_8;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__before_origin_length_complete: {
    switch (llh2__before_origin_length_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_error_11;
      default:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_altsvc_origin_length_complete;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_11: {
    state->error = 0x1;
    state->reason = "on_body_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_12: {
    state->error = 0x9;
    state->reason = "'on_body_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_end_llh2__internal__on_body_1: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = llh2__internal__on_body(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_1;
      return s_error;
    }
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_1;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_start_llh2__internal__on_body_1: {
    if (p == endp) {
      return s_n_llh2__internal__n_span_start_llh2__internal__on_body_1;
    }
    state->_span_pos0 = (void*) p;
    state->_span_cb0 = llh2__internal__on_body;
    goto s_n_llh2__internal__n_consume_length_1;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_14: {
    state->error = 0x18;
    state->reason = "Invalid length for WINDOW_UPDATE frame";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_12: {
    state->error = 0x1;
    state->reason = "on_window_increment_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_13: {
    state->error = 0x10;
    state->reason = "'on_window_increment_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_window_increment_complete: {
    switch (llh2__internal__on_window_increment_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_12;
      case 0:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
      default:
        goto s_n_llh2__internal__n_error_13;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_window_increment_uint_32be_byte4: {
    if (p == endp) {
      return s_n_llh2__internal__n_window_increment_uint_32be_byte4;
    }
    state->window_increment = (state->window_increment << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_window_increment_complete;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_window_increment_uint_32be_byte3: {
    if (p == endp) {
      return s_n_llh2__internal__n_window_increment_uint_32be_byte3;
    }
    state->window_increment = (state->window_increment << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_window_increment_uint_32be_byte4;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_window_increment_uint_32be_byte2: {
    if (p == endp) {
      return s_n_llh2__internal__n_window_increment_uint_32be_byte2;
    }
    state->window_increment = (state->window_increment << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_window_increment_uint_32be_byte3;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_window_increment_uint_32be: {
    if (p == endp) {
      return s_n_llh2__internal__n_window_increment_uint_32be;
    }
    state->window_increment = (*p);
    p++;
    goto s_n_llh2__internal__n_window_increment_uint_32be_byte2;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_is_equal_length: {
    switch (llh2__internal__c_is_equal_length(state, p, endp)) {
      case 0:
        goto s_n_llh2__internal__n_error_14;
      default:
        goto s_n_llh2__internal__n_window_increment_uint_32be;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_16: {
    state->error = 0x18;
    state->reason = "Invalid GOAWAY length";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_13: {
    state->error = 0x1;
    state->reason = "on_last_stream_id_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_15: {
    state->error = 0xe;
    state->reason = "'on_last_stream_id_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_last_stream_id_complete: {
    switch (llh2__internal__on_last_stream_id_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_13;
      case 0:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
      default:
        goto s_n_llh2__internal__n_error_15;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__check_goaway_frame_length: {
    switch (llh2__check_goaway_frame_length(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_error_16;
      default:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_last_stream_id_complete;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_18: {
    state->error = 0x18;
    state->reason = "PING frames must be a length of 8";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_14: {
    state->error = 0x1;
    state->reason = "on_body_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_17: {
    state->error = 0x9;
    state->reason = "'on_body_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_end_llh2__internal__on_body_2: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = llh2__internal__on_body(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_2;
      return s_error;
    }
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_2;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_start_llh2__internal__on_body_2: {
    if (p == endp) {
      return s_n_llh2__internal__n_span_start_llh2__internal__on_body_2;
    }
    state->_span_pos0 = (void*) p;
    state->_span_cb0 = llh2__internal__on_body;
    goto s_n_llh2__internal__n_consume_length_2;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_is_equal_length_1: {
    switch (llh2__internal__c_is_equal_length_1(state, p, endp)) {
      case 0:
        goto s_n_llh2__internal__n_error_18;
      default:
        goto s_n_llh2__internal__n_span_start_llh2__internal__on_body_2;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_21: {
    state->error = 0x1a;
    state->reason = "invalid promise_stream_id";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_16: {
    state->error = 0x1;
    state->reason = "on_promise_stream_id_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_20: {
    state->error = 0xd;
    state->reason = "'on_promise_stream_id_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_promise_stream_id_complete: {
    switch (llh2__internal__on_promise_stream_id_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_16;
      case 0:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
      default:
        goto s_n_llh2__internal__n_error_20;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__before_promise_stream_id_complete: {
    switch (llh2__before_promise_stream_id_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_error_21;
      default:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_promise_stream_id_complete;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_22: {
    state->error = 0x18;
    state->reason = "pad_length conflicts with length";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_15: {
    state->error = 0x1;
    state->reason = "on_pad_length_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_consume_pad_length;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_19: {
    state->error = 0x11;
    state->reason = "'on_pad_length_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_pad_length_complete: {
    switch (llh2__internal__on_pad_length_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_15;
      case 0:
        goto s_n_llh2__internal__n_consume_pad_length;
      default:
        goto s_n_llh2__internal__n_error_19;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pad_length_uint_8: {
    if (p == endp) {
      return s_n_llh2__internal__n_pad_length_uint_8;
    }
    state->pad_length = (*p);
    p++;
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_pad_length_complete;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__before_pad_length: {
    switch (llh2__before_pad_length(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_error_22;
      default:
        goto s_n_llh2__internal__n_pad_length_uint_8;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_test_flags: {
    switch (llh2__internal__c_test_flags(state, p, endp)) {
      case 0:
        goto s_n_llh2__internal__n_invoke_llh2__before_promise_stream_id_complete;
      default:
        goto s_n_llh2__internal__n_invoke_llh2__before_pad_length;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_17: {
    state->error = 0x1;
    state->reason = "on_settings_id pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_settings_value_uint_32be;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_18: {
    state->error = 0x1;
    state->reason = "on_settings_value_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__settings_subtract;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_24: {
    state->error = 0xc;
    state->reason = "'on_settings_value_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_settings_value_complete: {
    switch (llh2__internal__on_settings_value_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_18;
      case 0:
        goto s_n_llh2__internal__n_invoke_llh2__settings_subtract;
      default:
        goto s_n_llh2__internal__n_error_24;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_settings_value_uint_32be_byte4: {
    if (p == endp) {
      return s_n_llh2__internal__n_settings_value_uint_32be_byte4;
    }
    state->settings_value = (state->settings_value << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_settings_value_complete;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_settings_value_uint_32be_byte3: {
    if (p == endp) {
      return s_n_llh2__internal__n_settings_value_uint_32be_byte3;
    }
    state->settings_value = (state->settings_value << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_settings_value_uint_32be_byte4;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_settings_value_uint_32be_byte2: {
    if (p == endp) {
      return s_n_llh2__internal__n_settings_value_uint_32be_byte2;
    }
    state->settings_value = (state->settings_value << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_settings_value_uint_32be_byte3;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_23: {
    state->error = 0xb;
    state->reason = "'on_settings_id' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_settings_id: {
    switch (llh2__internal__on_settings_id(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_17;
      case 0:
        goto s_n_llh2__internal__n_settings_value_uint_32be;
      default:
        goto s_n_llh2__internal__n_error_23;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_store_settings_id: {
    switch (llh2__internal__c_store_settings_id(state, p, endp, match)) {
      default:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_settings_id;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_25: {
    state->error = 0x17;
    state->reason = "settings identifier is invalid";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_test_flags_1: {
    switch (llh2__internal__c_test_flags_1(state, p, endp)) {
      case 0:
        goto s_n_llh2__internal__n_ident;
      default:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_27: {
    state->error = 0x18;
    state->reason = "RST_STREAM must be a length of 4";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_19: {
    state->error = 0x1;
    state->reason = "on_error_code_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_26: {
    state->error = 0xf;
    state->reason = "'on_error_code_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_error_code_complete: {
    switch (llh2__internal__on_error_code_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_19;
      case 0:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
      default:
        goto s_n_llh2__internal__n_error_26;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_code_uint_32be_byte4: {
    if (p == endp) {
      return s_n_llh2__internal__n_error_code_uint_32be_byte4;
    }
    state->error_code = (state->error_code << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_error_code_complete;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_code_uint_32be_byte3: {
    if (p == endp) {
      return s_n_llh2__internal__n_error_code_uint_32be_byte3;
    }
    state->error_code = (state->error_code << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_error_code_uint_32be_byte4;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_code_uint_32be_byte2: {
    if (p == endp) {
      return s_n_llh2__internal__n_error_code_uint_32be_byte2;
    }
    state->error_code = (state->error_code << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_error_code_uint_32be_byte3;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_code_uint_32be: {
    if (p == endp) {
      return s_n_llh2__internal__n_error_code_uint_32be;
    }
    state->error_code = (*p);
    p++;
    goto s_n_llh2__internal__n_error_code_uint_32be_byte2;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_is_equal_length_2: {
    switch (llh2__internal__c_is_equal_length(state, p, endp)) {
      case 0:
        goto s_n_llh2__internal__n_error_27;
      default:
        goto s_n_llh2__internal__n_error_code_uint_32be;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_30: {
    state->error = 0x18;
    state->reason = "Priority data conflicts with length";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_20: {
    state->error = 0x1;
    state->reason = "on_dependency_id_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_stream_weight_uint_8;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_21: {
    state->error = 0x1;
    state->reason = "on_stream_weight_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_29: {
    state->error = 0x13;
    state->reason = "'on_stream_weight_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_stream_weight_complete: {
    switch (llh2__internal__on_stream_weight_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_21;
      case 0:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
      default:
        goto s_n_llh2__internal__n_error_29;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_28: {
    state->error = 0x12;
    state->reason = "'on_dependency_id_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_dependency_id_complete: {
    switch (llh2__internal__on_dependency_id_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_20;
      case 0:
        goto s_n_llh2__internal__n_stream_weight_uint_8;
      default:
        goto s_n_llh2__internal__n_error_28;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__post_dependency_id: {
    switch (llh2__post_dependency_id(state, p, endp)) {
      default:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_dependency_id_complete;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__before_priority: {
    switch (llh2__before_priority(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_error_30;
      default:
        goto s_n_llh2__internal__n_invoke_llh2__post_dependency_id;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_25: {
    state->error = 0x1;
    state->reason = "on_body_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_34: {
    state->error = 0x9;
    state->reason = "'on_body_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_end_llh2__internal__on_body_3: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = llh2__internal__on_body(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_3;
      return s_error;
    }
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_3;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_35: {
    state->error = 0x18;
    state->reason = "Priority data conflicts with length";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_23: {
    state->error = 0x1;
    state->reason = "on_dependency_id_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_stream_weight_uint_8_1;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_24: {
    state->error = 0x1;
    state->reason = "on_stream_weight_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_span_start_llh2__internal__on_body_3;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_33: {
    state->error = 0x13;
    state->reason = "'on_stream_weight_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_stream_weight_complete_1: {
    switch (llh2__internal__on_stream_weight_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_24;
      case 0:
        goto s_n_llh2__internal__n_span_start_llh2__internal__on_body_3;
      default:
        goto s_n_llh2__internal__n_error_33;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_32: {
    state->error = 0x12;
    state->reason = "'on_dependency_id_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_dependency_id_complete_1: {
    switch (llh2__internal__on_dependency_id_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_23;
      case 0:
        goto s_n_llh2__internal__n_stream_weight_uint_8_1;
      default:
        goto s_n_llh2__internal__n_error_32;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__post_dependency_id_1: {
    switch (llh2__post_dependency_id(state, p, endp)) {
      default:
        goto s_n_llh2__internal__n_invoke_llh2__internal__on_dependency_id_complete_1;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__before_priority_1: {
    switch (llh2__before_priority(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_error_35;
      default:
        goto s_n_llh2__internal__n_invoke_llh2__post_dependency_id_1;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_test_flags_3: {
    switch (llh2__internal__c_test_flags_3(state, p, endp)) {
      case 0:
        goto s_n_llh2__internal__n_span_start_llh2__internal__on_body_3;
      default:
        goto s_n_llh2__internal__n_invoke_llh2__before_priority_1;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_36: {
    state->error = 0x18;
    state->reason = "pad_length conflicts with length";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_22: {
    state->error = 0x1;
    state->reason = "on_pad_length_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_consume_pad_length_1;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_31: {
    state->error = 0x11;
    state->reason = "'on_pad_length_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_pad_length_complete_1: {
    switch (llh2__internal__on_pad_length_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_22;
      case 0:
        goto s_n_llh2__internal__n_consume_pad_length_1;
      default:
        goto s_n_llh2__internal__n_error_31;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pad_length_uint_8_1: {
    if (p == endp) {
      return s_n_llh2__internal__n_pad_length_uint_8_1;
    }
    state->pad_length = (*p);
    p++;
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_pad_length_complete_1;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__before_pad_length_1: {
    switch (llh2__before_pad_length(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_error_36;
      default:
        goto s_n_llh2__internal__n_pad_length_uint_8_1;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_test_flags_2: {
    switch (llh2__internal__c_test_flags(state, p, endp)) {
      case 0:
        goto s_n_llh2__internal__n_invoke_test_flags_3;
      default:
        goto s_n_llh2__internal__n_invoke_llh2__before_pad_length_1;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_27: {
    state->error = 0x1;
    state->reason = "on_body_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_38: {
    state->error = 0x9;
    state->reason = "'on_body_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_end_llh2__internal__on_body_4: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = llh2__internal__on_body(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_4;
      return s_error;
    }
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete_4;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_start_llh2__internal__on_body_4: {
    if (p == endp) {
      return s_n_llh2__internal__n_span_start_llh2__internal__on_body_4;
    }
    state->_span_pos0 = (void*) p;
    state->_span_cb0 = llh2__internal__on_body;
    goto s_n_llh2__internal__n_consume__sub_length_2;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__before_data_frame_body_start: {
    switch (llh2__before_data_frame_body_start(state, p, endp)) {
      default:
        goto s_n_llh2__internal__n_span_start_llh2__internal__on_body_4;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_39: {
    state->error = 0x18;
    state->reason = "pad_length conflicts with length";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_26: {
    state->error = 0x1;
    state->reason = "on_pad_length_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_consume_pad_length_2;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_37: {
    state->error = 0x11;
    state->reason = "'on_pad_length_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_pad_length_complete_2: {
    switch (llh2__internal__on_pad_length_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_26;
      case 0:
        goto s_n_llh2__internal__n_consume_pad_length_2;
      default:
        goto s_n_llh2__internal__n_error_37;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pad_length_uint_8_2: {
    if (p == endp) {
      return s_n_llh2__internal__n_pad_length_uint_8_2;
    }
    state->pad_length = (*p);
    p++;
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_pad_length_complete_2;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__before_pad_length_2: {
    switch (llh2__before_pad_length(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_error_39;
      default:
        goto s_n_llh2__internal__n_pad_length_uint_8_2;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_test_flags_4: {
    switch (llh2__internal__c_test_flags(state, p, endp)) {
      case 0:
        goto s_n_llh2__internal__n_invoke_llh2__before_data_frame_body_start;
      default:
        goto s_n_llh2__internal__n_invoke_llh2__before_pad_length_2;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_pause_5: {
    state->error = 0x1;
    state->reason = "on_body_complete pause";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_frame_end;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_5: {
    state->error = 0x9;
    state->reason = "'on_body_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_end_llh2__internal__on_body: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = llh2__internal__on_body(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete;
      return s_error;
    }
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_body_complete;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_span_start_llh2__internal__on_body: {
    if (p == endp) {
      return s_n_llh2__internal__n_span_start_llh2__internal__on_body;
    }
    state->_span_pos0 = (void*) p;
    state->_span_cb0 = llh2__internal__on_body;
    goto s_n_llh2__internal__n_consume_length;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_4: {
    state->error = 0x5;
    state->reason = "'on_stream_id_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_stream_id_complete: {
    switch (llh2__internal__on_stream_id_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_4;
      case 0:
        goto s_n_llh2__internal__n_invoke_load_type;
      default:
        goto s_n_llh2__internal__n_error_4;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_stream_id_int_32be_byte4: {
    if (p == endp) {
      return s_n_llh2__internal__n_stream_id_int_32be_byte4;
    }
    state->stream_id = (state->stream_id << 8) | ((*p) & 0x80);
    p++;
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_stream_id_complete;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_stream_id_int_32be_byte3: {
    if (p == endp) {
      return s_n_llh2__internal__n_stream_id_int_32be_byte3;
    }
    state->stream_id = (state->stream_id << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_stream_id_int_32be_byte4;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_stream_id_int_32be_byte2: {
    if (p == endp) {
      return s_n_llh2__internal__n_stream_id_int_32be_byte2;
    }
    state->stream_id = (state->stream_id << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_stream_id_int_32be_byte3;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_3: {
    state->error = 0x7;
    state->reason = "'on_flags_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_flags_complete: {
    switch (llh2__internal__on_flags_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_3;
      case 0:
        goto s_n_llh2__internal__n_stream_id_int_32be;
      default:
        goto s_n_llh2__internal__n_error_3;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_error_2: {
    state->error = 0x6;
    state->reason = "'on_type_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_type_complete: {
    switch (llh2__internal__on_type_complete(state, p, endp)) {
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
    state->reason = "'on_length_complete' callback error";
    state->error_pos = (const char*) p;
    state->_current = (void*)(intptr_t) s_error;
    return s_error;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_invoke_llh2__internal__on_length_complete: {
    switch (llh2__internal__on_length_complete(state, p, endp)) {
      case 1:
        goto s_n_llh2__internal__n_pause_1;
      case 0:
        goto s_n_llh2__internal__n_type_uint_8;
      default:
        goto s_n_llh2__internal__n_error_1;
    }
    UNREACHABLE;
  }
  s_n_llh2__internal__n_length_uint_24be_byte3: {
    if (p == endp) {
      return s_n_llh2__internal__n_length_uint_24be_byte3;
    }
    state->length = (state->length << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_invoke_llh2__internal__on_length_complete;
    UNREACHABLE;
  }
  s_n_llh2__internal__n_length_uint_24be_byte2: {
    if (p == endp) {
      return s_n_llh2__internal__n_length_uint_24be_byte2;
    }
    state->length = (state->length << 8) | (*p);
    p++;
    goto s_n_llh2__internal__n_length_uint_24be_byte3;
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
  
    error = ((llh2__internal__span_cb)state->_span_cb0)(state, state->_span_pos0, (const char*) endp);
    if (error != 0) {
      state->error = error;
      state->error_pos = endp;
      return error;
    }
  }
  
  return 0;
}