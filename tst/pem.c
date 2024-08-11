#include <stdlib.h>
#include <stdio.h>

#include <common.h>
#include <pem.h>
#include <codec.h>
#include <mtree.h>
#include <mproof.h>

signed
main (void) {

    size_t samples = 1000;
    size_t passed = 0;

    for ( size_t i = 0; i < samples; ++i ) {
        struct mtree * mt = NULL;
        struct textenc * mt_enc = NULL;
        struct mtree * mt_back = NULL;

        unsigned char * rt = NULL;
        struct textenc * rt_enc = NULL;
        unsigned char * rt_back = NULL;

        struct mproof * mp = NULL;
        struct textenc * mp_enc = NULL;
        struct mproof * mp_back = NULL;

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

        // init
        mt = create_mt(msg_count, msg_src, msg_loc, N);
        if ( !mt || mt->leaf_count != L || mt->hash_sz != N ) {
            goto cleanup;
        }
        if ( !(mt_enc = encode_mt(mt)) ) { goto cleanup; }
        if ( !(mt_back = decode_mt(mt_enc, NULL)) ) { goto cleanup; }

        bool mt_passed = mt->leaf_count == mt_back->leaf_count
                      && mt->hash_sz == mt_back->hash_sz
                      && mt->leaves && mt_back->leaves;
        for ( size_t l = 0; l < L; ++l ) {
            mt_passed &= mt->leaves[l].t == mt_back->leaves[l].t;
            if ( !mt_passed ) { goto cleanup; }
            mt_passed &= mt->leaves[l].sz == mt_back->leaves[l].sz;
            if ( !mt_passed ) { goto cleanup; }
            mt_passed &= !!mt->leaves[l].location == !!mt_back->leaves[l].location;
            if ( !mt_passed ) { goto cleanup; }
            if ( !!mt->leaves[l].location ) {
                mt_passed &= !strcmp(mt->leaves[l].location, mt_back->leaves[l].location);
                if ( !mt_passed ) { goto cleanup; }
            }
            mt_passed &= !memcmp(mt->leaves[l].data, mt_back->leaves[l].data, mt->leaves[l].sz);
            if ( !mt_passed ) { goto cleanup; }
        }

        if ( !(rt = root_from_tree(mt, 0)) ) { goto cleanup; }
        if ( !(rt_enc = encode_mr(rt, N)) ) { goto cleanup; }
        size_t back_sz = 0;
        if ( !(rt_back = decode_mr(rt_enc, &back_sz)) ) { goto cleanup; }
        if ( N != back_sz || memcmp(rt, rt_back, N) ) { goto cleanup; }

        mp = proof_from_tree(mt, 0, N, t);
        if ( !mp || mp->t != t || mp->hash_sz != N ) {
            goto cleanup;
        }
        if ( !(mp_enc = encode_mp(mp)) ) { goto cleanup; }
        if ( !(mp_back = decode_mp(mp_enc)) ) { goto cleanup; }
        bool mp_passed = mp->element_count == mp_back->element_count
                      && mp->hash_sz == mp_back->hash_sz
                      && mp->t == mp_back->t
                      && mp->msg_sz == mp_back->msg_sz
                      && mp->msg && mp_back->msg;
        mp_passed &= !memcmp(mp->msg, mp_back->msg, mp->msg_sz);
        for ( size_t e = 0; e < mp->element_count && mp_passed; ++e ) {
            mp_passed &= !memcmp(mp->elements[e], mp_back->elements[e], N);
        }

        ++passed;

        cleanup:
            if ( msg_count ) {
                for ( size_t i = 0; i < msg_count; ++i ) {
                    if ( msg_loc[i] ) { free(msg_loc[i]); }
                }
                if ( msg_src ) { free(msg_src); }
            }
            if ( rt ) { free(rt); }
            if ( rt_enc ) { free_txtenc(rt_enc); }
            if ( rt_back ) { free(rt_back); }

            if ( mp ) { free_proof(mp); }
            if ( mp_enc ) { free_txtenc(mp_enc); }
            if ( mp_back ) { free_proof(mp_back); }

            if ( mt ) { free_mt(mt); }
            if ( mt_enc ) { free_txtenc(mt_enc); }
            if ( mt_back ) { free_mt(mt_back); }
    };

    return passed == samples ? EXIT_SUCCESS : EXIT_FAILURE;
}
