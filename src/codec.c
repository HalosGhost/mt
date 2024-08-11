#include <codec.h>

struct textenc *
encode_mt (const struct mtree * mt) {

    if ( !mt ) { return NULL; }
    unsigned char * rt = root_from_tree(mt, 0);
    if ( !rt ) { return NULL; }

    struct textenc * enc = calloc(1, sizeof(struct textenc));
    if ( !enc ) { goto cleanup; }

    size_t lbl_sz = 0;
    FILE * l = open_memstream(&enc->label, &lbl_sz);
    fprintf(l, "NONSTD UNENCRYPTED B2B%zu STATIC MERKLE TREE", mt->hash_sz * 8);
    fclose(l);

    FILE * d = open_memstream((char ** )&enc->data, &enc->sz);
    fwrite(rt, 1, mt->hash_sz, d);
    uint64_t leaf_count_be = htonll(mt->leaf_count);
    fwrite(&leaf_count_be, 1, sizeof leaf_count_be, d);
    for ( size_t i = 0; i < mt->leaf_count; ++i ) {
        struct mt_msg leaf = mt->leaves[i];
        uint64_t msg_type = htonll(leaf.t);
        fwrite(&msg_type, 1, sizeof msg_type, d);
        if ( leaf.location ) {
            uint64_t loc_len = strlen(leaf.location) + 1;
            loc_len = htonll(loc_len);
            fwrite(&loc_len, sizeof loc_len, 1, d);
            fwrite(leaf.location, strlen(leaf.location) + 1, 1, d);
        } else {
            uint64_t zero_loc = 0;
            fwrite(&zero_loc, sizeof zero_loc, 1, d);
        }
        uint64_t sz = htonll(leaf.sz);
        fwrite(&sz, sizeof sz, 1, d);
        fwrite(leaf.data, leaf.sz, 1, d);
    }
    fclose(d);

    cleanup:
        crypto_wipe(rt, mt->hash_sz);
        free(rt);
        return enc;
}

struct mtree *
decode_mt (const struct textenc * enc, unsigned char ** rt) {

    if ( !enc ) { return NULL; }
    signed status = EXIT_SUCCESS;

    // parse hash_size from label
    size_t hash_bits = 0;
    sscanf(enc->label, "NONSTD UNENCRYPTED B2B%zu STATIC MERKLE TREE", &hash_bits);

    FILE * r = fmemopen(enc->data, enc->sz, "r");
    if ( !r ) { return NULL; }

    struct mtree * mt = calloc(1, sizeof *mt);
    if ( !mt ) { status = EXIT_FAILURE; goto cleanup; }

    mt->hash_sz = hash_bits / 8;
    if ( rt ) {
        *rt = calloc(1, mt->hash_sz);
        fread(*rt, mt->hash_sz, 1, r);
    } else {
        fseek(r, mt->hash_sz, SEEK_CUR);
    }

    uint64_t leaf_count = 0;
    fread(&leaf_count, sizeof leaf_count, 1, r);
    mt->leaf_count = ntohll(leaf_count);
    if ( !mt->leaf_count ) { status = EXIT_FAILURE; goto cleanup; }

    mt->leaves = calloc(mt->leaf_count, sizeof *mt->leaves);
    if ( !mt->leaves ) {
        status = EXIT_FAILURE; goto cleanup;
    }

    for ( size_t i = 0; i < mt->leaf_count; ++i ) {
        uint64_t msg_type = 0;
        fread(&msg_type, sizeof msg_type, 1, r);
        mt->leaves[i].t = (enum msg_type )ntohll(msg_type);
        uint64_t loc_len = 0;
        fread(&loc_len, sizeof loc_len, 1, r);
        loc_len = ntohll(loc_len);
        if ( loc_len ) {
            if ( loc_len > enc->sz ) {
                // nonsensical location size
                status = EXIT_FAILURE;
                goto cleanup;
            }
            mt->leaves[i].location = calloc(loc_len, sizeof *mt->leaves[i].location);
            fread(mt->leaves[i].location, loc_len, 1, r);
        }
        uint64_t sz = 0;
        fread(&sz, sizeof sz, 1, r);
        mt->leaves[i].sz = ntohll(sz);
        if ( mt->leaves[i].sz ) {
            if ( mt->leaves[i].sz > enc->sz ) {
                // nonsensical data size
                status = EXIT_FAILURE;
                goto cleanup;
            }

            mt->leaves[i].data = calloc(mt->leaves[i].sz, 1);
            if ( !mt->leaves[i].data ) {
                status = EXIT_FAILURE;
                goto cleanup;
            }
            fread(mt->leaves[i].data, mt->leaves[i].sz, 1, r);
        }
    }

    cleanup:
        if ( r ) { fclose(r); }
        if ( status == EXIT_SUCCESS ) {
            return mt;
        } else {
            if ( mt ) { free_mt(mt); }
            return NULL;
        }
}

struct textenc *
encode_mp (const struct mproof * mp) {

    struct textenc * enc = calloc(1, sizeof(struct textenc));
    if ( !enc ) { goto cleanup; }

    size_t lbl_sz = 0;
    FILE * l = open_memstream(&enc->label, &lbl_sz);
    fprintf(l, "NONSTD %sTACHED B2B%zu MERKLE PROOF",
        mp->t == of_knowledge ? "DE" : "AT",
        mp->hash_sz * 8);
    fclose(l);

    FILE * d = open_memstream((char ** )&enc->data, &enc->sz);
    uint64_t msg_sz_be = htonll(mp->msg_sz);
    fwrite(&msg_sz_be, 1, sizeof msg_sz_be, d);
    fwrite(mp->msg, 1, mp->msg_sz, d);
    uint64_t element_count_be = htonll(mp->element_count);
    fwrite(&element_count_be, 1, sizeof element_count_be, d);

    for ( size_t i = 0; i < mp->element_count; ++i ) {
        fwrite(mp->elements[i], mp->hash_sz, 1, d);
    }
    fclose(d);

    cleanup:
        return enc;
}

struct mproof *
decode_mp (const struct textenc * enc) {

    if ( !enc ) { return NULL; }
    signed status = EXIT_SUCCESS;

    // parse hash_size from label
    size_t hash_bits = 0;
    char tachment [3] = "";
    sscanf(enc->label, "NONSTD %2sTACHED B2B%zu MERKLE PROOF", tachment, &hash_bits);

    FILE * r = fmemopen(enc->data, enc->sz, "r");
    if ( !r ) { return NULL; }

    size_t i = 0; // avoids uninitialized-reads
    struct mproof * mp = calloc(1, sizeof *mp);
    if ( !mp ) { status = EXIT_FAILURE; goto cleanup; }

    mp->hash_sz = hash_bits / 8;
    mp->t = !strncmp(tachment, "DE", 2) ? of_knowledge : of_inclusion;
    uint64_t msg_sz_be = 0;
    fread(&msg_sz_be, sizeof msg_sz_be, 1, r);
    mp->msg_sz = ntohll(msg_sz_be);
    mp->msg = calloc(mp->msg_sz + 1, sizeof *mp->msg);
    if ( !mp->msg ) { status = EXIT_FAILURE; goto cleanup; }
    fread(mp->msg, mp->msg_sz, 1, r);
    uint64_t element_count_be = 0;
    fread(&element_count_be, 1, sizeof element_count_be, r);
    mp->element_count = ntohll(element_count_be);

    mp->elements = calloc(mp->element_count, sizeof *mp->elements);
    if ( !mp->elements ) { status = EXIT_FAILURE; goto cleanup; }

    for ( ; i < mp->element_count; ++i ) {
        mp->elements[i] = calloc(1, mp->hash_sz);
        if ( !mp->elements[i] ) { status = EXIT_FAILURE; goto cleanup; }
        fread(mp->elements[i], mp->hash_sz, 1, r);
    }

    cleanup:
        if ( r ) { fclose(r); }
        if ( status == EXIT_FAILURE ) {
            if ( mp ) {
                free_proof(mp);
            }

            return NULL;
        }
        return mp;
}

struct textenc *
encode_mr (const unsigned char * rt, size_t hash_sz) {

    struct textenc * enc = calloc(1, sizeof(struct textenc));
    if ( !enc ) { goto cleanup; }

    size_t lbl_sz = 0;
    FILE * l = open_memstream(&enc->label, &lbl_sz);
    fprintf(l, "NONSTD B2B%zu MERKLE ROOT", hash_sz * 8);
    fclose(l);

    FILE * d = open_memstream((char ** )&enc->data, &enc->sz);
    fwrite(rt, hash_sz, sizeof *rt, d);
    fclose(d);

    cleanup:
        return enc;
}

unsigned char *
decode_mr (const struct textenc * enc, size_t * sz) {

    if ( !enc ) { return NULL; }
    signed status = EXIT_SUCCESS;

    size_t hash_bits = 0;
    sscanf(enc->label, "NONSTD B2B%zu MERKLE ROOT", &hash_bits);

    FILE * r = fmemopen(enc->data, enc->sz, "r");
    if ( !r ) { return NULL; }

    unsigned char * rt = calloc(enc->sz, sizeof *rt);
    if ( !rt ) { status = EXIT_FAILURE; goto cleanup; }
    fread(rt, enc->sz, sizeof *rt, r);

    cleanup:
        if ( r ) { fclose(r); }
        if ( status == EXIT_FAILURE ) {
            if ( rt ) { crypto_wipe(rt, enc->sz); free(rt); }
            return NULL;
        } else {
            if ( sz ) {
                *sz = hash_bits / 8;
            }
        }
        return rt;
}

