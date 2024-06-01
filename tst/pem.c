#include <stdlib.h>
#include <stdio.h>

#include <common.h>
#include <pem.h>

signed
main (void) {

    signed res = 1;

    size_t samples = 100;
    size_t passed = 0;
    size_t failed = 0;

    for ( size_t i = 0; i < samples; ++i ) {
        struct textenc t = {
            .label = "EXAMPLE",
            .data = (unsigned char *)("things and stuff"),
            .sz = sizeof("things and stuff")
        };

        char * fore = NULL;
        size_t sz = 0;

        FILE * w = open_memstream(&fore, &sz);
        size_t fore_written = fw_txtenc(w, &t);
        fclose(w);

        FILE * r = fmemopen(fore, sz, "r");
        struct textenc * back = fr_txtenc(r);
        fclose(r);

        res = fore_written && back
           && t.sz == back->sz
           && !memcmp(t.data, back->data, t.sz)
           && !memcmp(t.label, back->label, 7);

        if ( res ) {
            ++passed;
        } else {
            ++failed;
        }

        free(fore);
        free(back);
    };

    return passed == samples ? EXIT_SUCCESS : EXIT_FAILURE;
}
