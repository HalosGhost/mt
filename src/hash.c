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

signed
const_cmp (const unsigned char * a, size_t an, const unsigned char * b, size_t bn) {

    if ( an != bn ) { return -1; }

    unsigned char acc = 0;
    for ( size_t i = 0; i < an; ++i ) {
        acc |= a[i] ^ b[i];
    }

    return !!acc;
}
