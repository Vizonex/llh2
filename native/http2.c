#include "llh2.h"
#include "stdio.h"

int llh2__settings_subtract (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp){
    /* subtraction */
    /* will use s->_sub_length to perform our subtractions with */
    if (!s->_sub_length){
        s->_sub_length = s->length;
    }
    s->_sub_length -= 6;
    /* if s->_sub_length is greater than zero return 1 else 0 */
    return s->_sub_length > 0;
};


int llh2__check_goaway_frame_length(
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp
){
    if (s->length < 8){
        return -1;
    }
    s->_sub_length = s->length - 8;
    return 0;
}

int llh2__before_origin_length_complete(
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp){
    if (s->altsvc_origin_length > s->length){
        return 1;
    };
    s->_sub_length = s->length - s->altsvc_origin_length;
    return 0;
}

int llh2__before_promise_stream_id_complete (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp){
    if ((s->promise_stream_id == 0) || (s->promise_stream_id % 2 == 0)){
        return 1;
    }
    s->_sub_length = s->length - 4;
    return 0;
};

int llh2__before_pad_length (
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp){
    if ((*p) > s->length){
        /* CONFLICTING DATA! */
        return 1;
    }
    /* subtract before feeding off to body */
    if (s->_sub_length){
        s->_sub_length -= (uint32_t)(*p);
    }
    return 0;
}

int llh2__post_dependency_id(
    llh2__internal_t* s, const unsigned char* p,
    const unsigned char* endp
){
    s->is_exclusive = (s->dependency_id >> 31) ? 1: 0;
    s->dependency_id &= 0x7FFFFFFF;
    return 0;
}

int llh2__before_data_frame_body_start(
    llh2_t* s, const unsigned char *p,
    const unsigned char *endp){
    s->_sub_length = s->length - s->pad_length;
    return 0;
}

int llh2__before_priority(
    llh2_t* s, const unsigned char *p,
    const unsigned char *endp){
    if (s->length >= 5){
        s->_sub_length = s->length - 5;
        return 0;
    }
    /* FAIL */
    return 1;
}

void llh2__internal_debug(llh2_t *s, const char *p, const char *endp,
                 const char *msg)
{
    if (p == endp)
    {
        fprintf(stderr, "p=%p type=%d flags=%02x stream_id=%d next=null debug=%s\n", s, s->type,
                s->flags, s->stream_id, msg);
    }
    else
    {
        fprintf(stderr, "p=%p type=%d flags=%02x stream_id=%d next=%02x   debug=%s\n", s,
                s->type, s->flags, s->stream_id, *p, msg);
    }
}
