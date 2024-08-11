#include <stdlib.h>
#include <stdio.h>

#include <common.h>
#include <pem.h>
#include <mtree.h>
#include <mproof.h>

signed
main (void) {

    size_t samples = 1000;
    size_t passed = 0;

    for ( size_t i = 0; i < samples; ++i ) {
        struct mtree * mt = NULL;
        enum msg_type * msg_src = NULL;
        char ** msg_loc = NULL;
        size_t msg_count = 0;

        // L is 2, 4, 8, or 16
        size_t L = 0;
        while ( !L ) {
            randombytes(&L, 1);
            L &= ~((~0ULL) << 2);
            L = 2 << L;
        }

        // include L uniformly-random N-byte messages
        for ( size_t l = 0; l < L; ++l ) {
            ++msg_count;
            msg_loc = realloc(msg_loc, msg_count * sizeof *msg_loc);
            msg_src = realloc(msg_src, msg_count * sizeof *msg_src);
            msg_loc[msg_count - 1] = NULL;
            msg_src[msg_count - 1] = decoy;
        }

        enum proof_type t = of_knowledge;
        size_t N = 0;
        while ( !N ) {
            randombytes(&N, 1);
            size_t trimmed = N & ~((~0ULL) << 6);
            if ( trimmed ) {
                if ( N & (1 << 6) ) {
                    t = of_inclusion;
                }
                N = trimmed;
            } else {
                N = 0;
            }
        }

        mt = create_mt(msg_count, msg_src, msg_loc, N);

        unsigned char * rt = root_from_tree(mt, 0);
        bool all_leaves_valid = L == msg_count;
        for ( size_t p = 0; p < L && all_leaves_valid; ++p ) {
            struct mproof * prf = proof_from_tree(mt, p, N, t);
            bool res = is_valid_proof(prf);
            all_leaves_valid &= res;
            free_proof(prf);
        }

        passed += all_leaves_valid;

        if ( msg_count ) {
            for ( size_t i = 0; i < msg_count; ++i ) {
                if ( msg_loc[i] ) { free(msg_loc[i]); }
            }
            if ( msg_src ) { free(msg_src); }
        }
        if ( rt ) { free(rt); }
        if ( mt ) { free_mt(mt); }
    };

    return passed == samples ? EXIT_SUCCESS : EXIT_FAILURE;
}
