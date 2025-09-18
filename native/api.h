#ifndef __LLH2_API_H__
#define __LLH2_API_H__

/* Temporarly */
#include "llh2.h"
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
    llh2_data_cb on_body;
    /* Called first thing when a frame is ready for parsing */
    
    llh2_cb on_frame_start;
    llh2_cb on_frame_end;
    /* Called when length of body has been obtained */

    llh2_cb on_length;

    /* type obtained */
    llh2_cb on_type;

    /* flags obtained */
    llh2_cb on_flags;

    llh2_cb on_body_start;
    llh2_cb on_body_end;
    llh2_cb on_reset;
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


#ifdef __cplusplus
}
#endif

#endif // __LLH2_API_H__