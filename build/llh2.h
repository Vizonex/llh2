#ifndef LLH2_C_HEADERS__H__
#define LLH2_C_HEADERS__H__
#ifdef __cplusplus
extern "C" {
#endif

enum llh2_reset_flag {
  H2RF_ERROR = -1,
  H2RF_PAUSE = 0,
  H2RF_EXIT = 1
};
typedef enum llh2_reset_flag llh2_reset_flag_t;

enum llh2_errno {
  H2PE_OK = 0,
  H2PE_PAUSED = 1,
  H2PE_CB_ON_FRAME_START = 2,
  H2PE_CB_ON_FRAME_END = 3,
  H2PE_CB_ON_LENGTH = 4,
  H2PE_CB_ON_STREAM_ID = 5,
  H2PE_CB_ON_TYPE = 6,
  H2PE_CB_ON_FLAG = 7,
  H2PE_CB_ON_BODY_START = 8,
  H2PE_CB_ON_BODY_END = 9,
  H2PE_CB_ON_RESET = 10,
  H2PE_CB_ON_SETTINGS_IDENTIFIER = 11,
  H2PE_CB_ON_SETTINGS_VALUE = 12,
  H2PE_CB_ON_PROMISE_STREAM_ID = 13,
  H2PE_INVALID_SETTINGS_IDENTIFIER = 14,
  H2PE_INVALID_LENGTH = 15,
  H2PE_INVALID_PAD_LENGTH = 16,
  H2PE_EXIT = 17,
  H2PE_USER = 18
};
typedef enum llh2_errno llh2_errno_t;

enum llh2_misc_flags {
  H2MF_NONE = 0,
  H2MF_ACK = 1
};
typedef enum llh2_misc_flags llh2_misc_flags_t;

enum llh2_stream_flags {
  H2SF_NONE = 0,
  H2SF_END_STREAM = 1,
  H2SF_END_HEADERS = 4,
  H2SF_PADDED = 8,
  H2SF_PRIORITY = 32
};
typedef enum llh2_stream_flags llh2_stream_flags_t;

enum llh2_frame_type {
  H2FT_DATA = 0x0,
  H2FT_HEADERS = 0x1,
  H2FT_PRIORITY = 0x2,
  H2FT_RST_STREAM = 0x3,
  H2FT_SETTINGS = 0x4,
  H2FT_PUSH_PROMISE = 0x5,
  H2FT_PING = 0x6,
  H2FT_GOAWAY = 0x7,
  H2FT_WINDOW_UPDATE = 0x8,
  H2FT_CONTINUATION = 0x9
};
typedef enum llh2_frame_type llh2_frame_type_t;

#define LLH2_FRAME_TYPE_MAP_MAP(XX) \
  XX(9, CONTINUATION, CONTINUATION) \
  XX(0, DATA, DATA) \
  XX(7, GOAWAY, GOAWAY) \
  XX(1, HEADERS, HEADERS) \
  XX(6, PING, PING) \
  XX(2, PRIORITY, PRIORITY) \
  XX(5, PUSH_PROMISE, PUSH_PROMISE) \
  XX(3, RST_STREAM, RST_STREAM) \
  XX(4, SETTINGS, SETTINGS) \
  XX(8, WINDOW_UPDATE, WINDOW_UPDATE) \


#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif  /* LLH2_C_HEADERS__H__ */
#ifndef INCLUDE_LLH2_ITSELF_H_
#define INCLUDE_LLH2_ITSELF_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct llh2__internal_s llh2__internal_t;
struct llh2__internal_s {
  int32_t _index;
  void* _span_pos0;
  int32_t error;
  const char* reason;
  const char* error_pos;
  void* data;
  void* _current;
  void*  settings;
  uint32_t  length;
  uint32_t  _sub_length;
  uint32_t  stream_id;
  uint8_t  type;
  uint8_t  flags;
  uint8_t  h2_state;
  uint8_t  h2_flag;
  uint8_t  weight;
  uint16_t  setting_ident;
  uint32_t  setting_value;
  uint8_t  pad_length;
  uint32_t  promise_stream_id;
};
int llh2__internal_init(llh2__internal_t* s);
int llh2__internal_execute(llh2__internal_t* s, const char* p, const char* endp);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* INCLUDE_LLH2_ITSELF_H_ */