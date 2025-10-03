
#ifndef INCLUDE_LLH2_VERSION_H_
#define INCLUDE_LLH2_VERSION_H_

#define LLH2_VERSION_MAJOR 0
#define LLH2_VERSION_MINOR 0
#define LLH2_VERSION_PATCH 1

#endif /* INCLUDE_LLH2_VERSION_H_ */
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
  H2PE_CB_ON_SETTINGS_ID = 11,
  H2PE_CB_ON_SETTINGS_VALUE = 12,
  H2PE_CB_ON_PROMISE_STREAM_ID = 13,
  H2PE_CB_ON_LAST_STREAM_ID = 14,
  H2PE_CB_ON_ERROR_CODE = 15,
  H2PE_CB_ON_WINDOW_INCREMENT = 16,
  H2PE_CB_ON_PAD_LENGTH = 17,
  H2PE_CB_ON_DEPENDENCY_ID = 18,
  H2PE_CB_ON_STREAM_WEIGHT = 19,
  H2PE_CB_ON_ALTSVC_ORIGIN_LENGTH = 20,
  H2PE_CB_ON_ALTSVC_ORIGIN_VALUE = 21,
  H2PE_CB_ON_ALTSVC_FIELD_VALUE = 22,
  H2PE_INVALID_SETTINGS_IDENTIFIER = 23,
  H2PE_INVALID_LENGTH = 24,
  H2PE_INVALID_PAD_LENGTH = 25,
  H2PE_INVALID_PROMISE_STREAM_ID = 26,
  H2PE_INVALID_PUSH_PROMISE_BODY = 27,
  H2PE_INVALID_HEADERS_FRAME = 28,
  H2PE_EXIT = 29,
  H2PE_USER = 30
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
  H2FT_CONTINUATION = 0x9,
  H2FT_ALTSVC = 0xa
};
typedef enum llh2_frame_type llh2_frame_type_t;

#define LLH2_FRAME_TYPE_MAP_MAP(XX) \
  XX(10, ALTSVC, ALTSVC) \
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
  void* _span_cb0;
  int32_t error;
  const char* reason;
  const char* error_pos;
  void* data;
  void* _current;
  void*  settings;
  uint32_t  length;
  uint32_t  stream_id;
  uint8_t  type;
  uint8_t  flags;
  uint8_t  is_exclusive;
  uint32_t  _sub_length;
  uint8_t  pad_length;
  uint8_t  stream_weight;
  uint32_t  dependency_id;
  uint32_t  error_code;
  uint16_t  settings_id;
  uint32_t  settings_value;
  uint32_t  promise_stream_id;
  uint32_t  last_stream_id;
  uint32_t  window_increment;
  uint16_t  altsvc_origin_length;
};
int llh2__internal_init(llh2__internal_t* s);
int llh2__internal_execute(llh2__internal_t* s, const char* p, const char* endp);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* INCLUDE_LLH2_ITSELF_H_ */


#ifndef __LLH2_CWRAPPER_SECTION
#define __LLH2_CWRAPPER_SECTION

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__wasm__)
#define LLH2_EXPORT __attribute__((visibility("default")))
#elif defined(_WIN32)
#define LLH2_EXPORT __declspec(dllexport)
#else
#define LLH2_EXPORT
#endif

typedef llh2__internal_t llh2_t;
typedef struct llh2_settings_s llh2_settings_t;

typedef int (*llh2_data_cb)(llh2_t*, const char *at, size_t length);
typedef int (*llh2_cb)(llh2_t*);

struct llh2_settings_s {
  /* DATA CALLBACKS */

  llh2_data_cb on_body;
  llh2_data_cb altsvc_origin_value;
  llh2_data_cb altsvc_field_value;

  /* CALLBACKS */

  llh2_cb on_reset;
  llh2_cb on_frame_start;
  llh2_cb on_frame_end;
  llh2_cb on_length_complete;
  llh2_cb on_stream_id_complete;
  llh2_cb on_type_complete;
  llh2_cb on_flags_complete;
  llh2_cb on_pad_length_complete;
  llh2_cb on_body_complete;
  llh2_cb on_stream_weight_complete;
  llh2_cb on_dependency_id_complete;
  llh2_cb on_error_code_complete;
  llh2_cb on_settings_id;
  llh2_cb on_settings_value_complete;
  llh2_cb on_promise_stream_id_complete;
  llh2_cb on_last_stream_id_complete;
  llh2_cb on_window_increment_complete;
  llh2_cb on_altsvc_origin_length_complete;
  llh2_cb altsvc_origin_value_complete;
  llh2_cb altsvc_field_value_complete;
};

/// @brief Initalizes a new http/2 parser
/// @param parser the parser to initalize
/// @param settings the callbacks to give to this parser
LLH2_EXPORT
void llh2_init(llh2_t* parser, const llh2_settings_t* settings);

/// @brief resets parser excluding the setting callbacks if any were given
/// @param parser the parser to be reset
LLH2_EXPORT
void llh2_reset(llh2_t* parser);

/// @brief obtains the type from the given frame if any are provided
/// @param parser the parser to obtain the frame's type from
/// @return the type of frame 
LLH2_EXPORT 
uint8_t llh2_get_type(llh2_t* parser);

/// @brief Obtains the frame's flags from the given parser
/// @param parser the parser to obtain the flags from
/// @return the parsed frame's flags
LLH2_EXPORT
uint8_t llh2_get_flags(llh2_t* parser);

/// @brief Obtains the frame's body/payload size
/// @param parser the parser to obtain the length of the body
/// @return the parsed frame's payload size
LLH2_EXPORT
uint32_t llh2_get_length(llh2_t* parser);

/// @brief Obtains the stream_id for the given frame
/// @param parser the parser to obtain the stream_id from
/// @return the parsed frame's stream_id
LLH2_EXPORT
uint32_t llh2_get_stream_id(llh2_t* parser);

/// @brief Resumes parser if it has been paused.
/// @param parser the parser to unpase and resume parsing data with.
LLH2_EXPORT
void llh2_resume(llh2_t* parser);

/// @brief Execute the parser and give datastream of frames to parse
/// @param parser the parser to execute data from
/// @param data the data that will be parsed
/// @param len the length of the data stream
/// @return errono if raised otherwise this will result in H2PE_OK which is 0
LLH2_EXPORT 
llh2_errno_t llh2_execute(llh2_t* parser, const char* data, size_t len);

/// @brief Get String Name of the Provided type
/// @return "UNKNOWN" if type is unknown otherwise return the Frame's Name
LLH2_EXPORT
const char* llh2_get_type_name(llh2_t* parser);

LLH2_EXPORT
void llh2_settings_init(llh2_settings_t* settings);

#ifdef __cplusplus
}
#endif

#endif // __LLH2_CWRAPPER_SECTION
