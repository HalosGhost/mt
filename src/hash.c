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

unsigned char *
to_hex (const unsigned char * buf, size_t sz) {

    unsigned char * hex = calloc(sz * 2 + 1, sizeof(*hex));
    for ( size_t i = 0; i < sz; ++i ) {
        sprintf(hex + (i * 2), "%02x", buf[i]);
    }

    return hex;
}

