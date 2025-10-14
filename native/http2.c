#include <stddef.h>
#include <stdio.h>

#include "llh2.h"

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
    if ((s->promise_stream_id == 0) || ((s->promise_stream_id % 2) != 0)){
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
    s->dependency_id = (uint32_t)(s->dependency_id & 0x7FFFFFFF);
    return 0;
}

int llh2__before_data_frame_body_start(
    llh2_t* s, const unsigned char *p,
    const unsigned char *endp){
    s->_sub_length = (s->pad_length) ? (s->length - (s->pad_length + 1)) : s->length;
    return 0;
}

int llh2__before_push_promise_frame_body_start(
    llh2_t* s, const unsigned char *p,
    const unsigned char *endp){
    s->_sub_length = (s->pad_length) ? (s->length - (s->pad_length + 1)) : s->length;
    if (s->_sub_length < 4){
        /* BAD! */
        return -1;
    };
    s->_sub_length -= 4;
    return 0;
}

int llh2__before_headers_frame_body(
    llh2_t* s, const unsigned char *p,
    const unsigned char *endp){
    /* did we visit priotiry? if yes, subtract 5 */
    if ((!s->_sub_length) && (s->flags & H2SF_PRIORITY)){
        /* There should no body to parse if this is the case... */
        return 1;
    }
    if ((!(s->stream_weight)) && (!(s->pad_length))){
        /* give the full length */
        s->_sub_length = s->length;
        /* mark that we have data... */
        return 0;
    }
    if (s->stream_weight){
        s->_sub_length -= 5;
    }
    if (s->pad_length){
        s->_sub_length -= (s->pad_length);
    }
    
    return (s->_sub_length < 0) ? 1: 0;
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

int llh2__before_field_start(
    llh2_t* s, const unsigned char *p,
    const unsigned char *endp)
{
    /* We need to preform a little bit of subtraction
       will use a signed value to help with capturing bad values
       that could be passed by an attacker */
    if ((int)(s->length - (s->altsvc_origin_length + 2)) < 0){
        /* BAD! */
        return -1;
    }
    s->_sub_length = s->length - (s->altsvc_origin_length + 2);
    return 0;
}

void llh2__internal_debug(llh2_t *s, const char *p, const char *endp,
                 const char *msg)
{
    /* Check if p is NULL we don't want to accidently cause a crash now do we? */
    if ((*p == '\x00') || (p == endp))
    {
        fprintf(stderr, "p=%p type=%d flags=%02x stream_id=%d length=%d sublength=%d altsrvc-o-len=%d next=null debug=%s\n", s, s->type,
                s->flags, s->stream_id, llh2_get_length(s), s->_sub_length,  s->altsvc_origin_length,  msg);
    }
    else
    {
        fprintf(stderr, "p=%p type=%d flags=%02x stream_id=%d length=%d sublength=%d altsrvc-o-len=%d next=%02x  debug=%s\n", s,
                s->type, s->flags, s->stream_id, llh2_get_length(s), s->_sub_length, s->altsvc_origin_length, *p, msg);
    }
}
