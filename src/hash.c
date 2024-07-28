#include <hash.h>
#include <stdio.h>

signed
get_hash (
    const unsigned char * msg,
    size_t sz,
    unsigned char * out,
    size_t bytes
) {

    if ( !msg || !out || bytes > 64 ) {
        return -1;
    }

    crypto_blake2b_general(out, bytes, NULL, 0, msg, sz);

    return bytes;
}

