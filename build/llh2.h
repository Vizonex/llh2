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
  H2PE_LENGTH_OVERFLOW = 2,
  H2PE_STREAM_ID_OVERFLOW = 3,
  H2PE_CB_ON_FRAME_START = 4,
  H2PE_CB_ON_FRAME_END = 5,
  H2PE_CB_ON_LENGTH = 6,
  H2PE_CB_ON_STREAM_ID = 7,
  H2PE_CB_ON_TYPE = 8,
  H2PE_CB_ON_FLAG = 9,
  H2PE_CB_ON_BODY_START = 10,
  H2PE_CB_ON_BODY_END = 11,
  H2PE_CB_ON_RESET = 12,
  H2PE_EXIT = 13,
  H2PE_USER = 14
};
typedef enum llh2_errno llh2_errno_t;

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
  uint32_t  stream_id;
  uint8_t  type;
  uint8_t  flags;
};
int llh2__internal_init(llh2__internal_t* s);
int llh2__internal_execute(llh2__internal_t* s, const char* p, const char* endp);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* INCLUDE_LLH2_ITSELF_H_ */