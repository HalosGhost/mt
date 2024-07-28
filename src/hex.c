#include <hex.h>

unsigned char *
to_hex (const unsigned char * buf, size_t sz) {

    unsigned char * hex = calloc(sz * 2 + 1, sizeof(*hex));
    for ( size_t i = 0; i < sz; ++i ) {
        sprintf((char * )(hex + (i * 2)), "%02hhx", buf[i]);
    }

    return hex;
}

unsigned char *
from_hex (const unsigned char * hex, size_t sz) {

    unsigned char * buf = calloc(sz / 2 + 1, sizeof(*hex));
    for ( size_t i = 0; i < (sz / 2); ++i ) {
        sscanf((char * )(hex + (i * 2)), "%2hhx", buf + i);
    }

    return buf;
}

