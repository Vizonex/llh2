

#include "llh2.h"
#include <stdio.h>

/* Unlike llhttp a decision early on was made to Autogenerate 
 * this section of code due to it's increase in size comapred to llhttp 
 * due to having more callbacks to write in or change overtime as 
 * new things get introdued to the http/2 protocol as it's still a 
 * bit newer and not completely mature yet.
 */

// Don't want to override llhttp's work now do we?
#define H2_CALLBACK_MAYBE(PARSER, NAME)                         \
    do {                                                        \
        const llh2_settings_t *settings;                        \
        settings = (const llh2_settings_t *)(PARSER)->settings; \
        if (settings == NULL || settings->NAME == NULL)         \
        {                                                       \
            err = 0;                                            \
            break;                                              \
        }                                                       \
        err = settings->NAME((PARSER));                         \
    } while (0)

#define H2_SPAN_CALLBACK_MAYBE(PARSER, NAME, START, LEN)                      \
    do {                                                                      \
        const llh2_settings_t *settings;                                      \
        settings = (const llh2_settings_t *)(PARSER)->settings;               \
        if (settings == NULL || settings->NAME == NULL)                       \
        {                                                                     \
            err = 0;                                                          \
            break;                                                            \
        }                                                                     \
        err = settings->NAME((PARSER), (START), (LEN));                       \
        if (err == -1)                                                        \
        {                                                                     \
            err = H2PE_USER;                                                  \
            (PARSER)->reason = "Span callback error in " #NAME;               \
        }                                                                     \
    } while (0)


/* AUTO GENERATED CALLBACKS PLEASE DO NOT EDIT!!! */

int llh2__internal__on_reset (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_reset);
    return err;
};


int llh2__internal__on_frame_start (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_frame_start);
    return err;
};


int llh2__internal__on_frame_end (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_frame_end);
    return err;
};


int llh2__internal__on_length_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_length_complete);
    return err;
};


int llh2__internal__on_stream_id_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_stream_id_complete);
    return err;
};


int llh2__internal__on_type_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_type_complete);
    return err;
};


int llh2__internal__on_flags_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_flags_complete);
    return err;
};


/* === Span Callback on_body === */

int llh2__internal__on_body (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_SPAN_CALLBACK_MAYBE(s, on_body, p, endp - p);
    return err;
};

int llh2__internal__on_body_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_body_complete);
    return err;
};


int llh2__internal__on_pad_length_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_pad_length_complete);
    return err;
};


int llh2__internal__on_stream_weight_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_stream_weight_complete);
    return err;
};


int llh2__internal__on_dependency_id_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_dependency_id_complete);
    return err;
};


int llh2__internal__on_error_code_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_error_code_complete);
    return err;
};


int llh2__internal__on_settings_id (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_settings_id);
    return err;
};


int llh2__internal__on_settings_value_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_settings_value_complete);
    return err;
};


int llh2__internal__on_promise_stream_id_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_promise_stream_id_complete);
    return err;
};


int llh2__internal__on_last_stream_id_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_last_stream_id_complete);
    return err;
};


int llh2__internal__on_window_increment_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_window_increment_complete);
    return err;
};


int llh2__internal__on_altsvc_origin_length_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_altsvc_origin_length_complete);
    return err;
};


/* === Span Callback altsvc_origin_value === */

int llh2__internal__altsvc_origin_value (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_SPAN_CALLBACK_MAYBE(s, altsvc_origin_value, p, endp - p);
    return err;
};

int llh2__internal__altsvc_origin_value_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, altsvc_origin_value_complete);
    return err;
};


/* === Span Callback altsvc_field_value === */

int llh2__internal__altsvc_field_value (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_SPAN_CALLBACK_MAYBE(s, altsvc_field_value, p, endp - p);
    return err;
};

int llh2__internal__altsvc_field_value_complete (
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, altsvc_field_value_complete);
    return err;
};





/* API */

void llh2_init(llh2_t *parser, const llh2_settings_t *settings) {
    llh2__internal_init(parser);
    /*  Unlike llhttp we reserve type for handling frames as 
        in the frame type according to RFC 9113 */
    parser->settings = (void *)settings;
}

void llh2_reset(llh2_t *parser) {
    void* data = parser->data;
    llh2_init(parser, (llh2_settings_t*)parser->settings);
    parser->data = data;
}

uint8_t llh2_get_type(llh2_t *parser) {
    return parser->type;
}

uint8_t llh2_get_flags(llh2_t *parser)
{
    return parser->flags;
}

uint32_t llh2_get_length(llh2_t *parser)
{
    return parser->length;
}

uint32_t llh2_get_stream_id(llh2_t *parser)
{
    return parser->stream_id;
}

void llh2_resume(llh2_t *parser)
{
    if (parser->error != H2PE_PAUSED){
        return;
    }
    parser->error = 0;
}

llh2_errno_t llh2_execute(llh2_t *parser, const char *data, size_t len)
{
    return (llh2_errno_t)llh2__internal_execute(parser, data, data + len);
}

const char* llh2_get_type_name(llh2_t* parser){
    switch (parser->type){
        #define __LLH2_TYPE_NAME_CASE(ID, NAME, _) case ID: return #NAME;
        LLH2_FRAME_TYPE_MAP_MAP(__LLH2_TYPE_NAME_CASE)
        #undef __LLH2_TYPE_NAME_CASE
        default: 
            return "UNKNOWN";
    }
}

/* Private */

void llh2__internal_debug(llh2_t *s, const char *p, const char *endp,
                 const char *msg)
{
    if (p == endp)
    {
        fprintf(stderr, "p=%p type=%d flags=%02x next=null debug=%s\n", s, s->type,
                s->flags, msg);
    }
    else
    {
        fprintf(stderr, "p=%p type=%d flags=%02x next=%02x   debug=%s\n", s,
                s->type, s->flags, *p, msg);
    }
}




