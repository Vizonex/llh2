#include "api.h"
#include <stdio.h>

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
            llh2_set_error_reason((PARSER), "Span callback error in " #NAME); \
        }                                                                     \
    } while (0)

int llh2__on_body(
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_SPAN_CALLBACK_MAYBE(s, on_body, p, endp - p);
    return err;
};

int llh2__on_frame_start(
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_frame_start);
    return err;
};

int llh2__on_length(
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_length);
    return err;
};

int llh2__on_type(
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_type);
    return err;
};

int llh2__on_flags(
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_flags);
    return err;
};

int llh2__on_body_start(
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_body_start);
    return err;
};

int llh2__on_body_end(
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_body_end);
    return err;
};

int llh2__on_frame_end(
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_frame_end);
    return err;
};

int llh2__on_reset(
    llh2_t *s, const unsigned char *p,
    const unsigned char *endp){
    int err;
    H2_CALLBACK_MAYBE(s, on_reset);
    return err;
};

/* API */

void llh2_init(llh2_t *parser, const llh2_settings_t *settings)
{
    llh2__internal_init(parser);
    /*  Unlike llhttp we reserve type for handling frames as 
        in the frame type according to RFC 9113 */
    parser->settings = (void *)settings;
}

void llh2_reset(llh2_t *parser)
{
    void* data = parser->data;
    llh2_init(parser, (llh2_settings_t*)parser->settings);
    parser->data = data;
}

uint8_t llh2_get_type(llh2_t *parser)
{
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

