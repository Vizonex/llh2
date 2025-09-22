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