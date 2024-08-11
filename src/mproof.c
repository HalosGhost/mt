#include <mproof.h>

struct mproof *
proof_from_tree (
    const struct mtree * mt,
    size_t leaf_idx,
    size_t hash_len,
    enum proof_type type
) {

    if ( !mt || leaf_idx >= mt->leaf_count ) {
        return NULL;
    }

    if ( !hash_len || hash_len > 64 ) {
        hash_len = mt->hash_sz;
    }

    struct mt_msg leaf = mt->leaves[leaf_idx];

    struct mproof * p = calloc(1, sizeof *p);
    p->t = type;
    switch ( type ) {
        case of_inclusion:
            p->msg_sz = leaf.sz;
            p->msg = calloc(1, p->msg_sz);
            memcpy(p->msg, leaf.data, p->msg_sz);
            break;

        case of_knowledge: {
            unsigned char * buf = calloc(1, leaf.sz + 1);
            memcpy(buf + 1, leaf.data, leaf.sz);
            p->msg_sz = hash_len;
            p->msg = calloc(1, hash_len);
            get_hash(buf, leaf.sz + 1, p->msg, hash_len);
            crypto_wipe(buf, leaf.sz + 1);
            free(buf);
        } break;
    }

    p->hash_sz = hash_len;
    p->element_count = log2(mt->leaf_count);
    p->elements = calloc(p->element_count + 1, sizeof *p->elements);

    unsigned char *** tiers = materialize_tree(mt, hash_len);
    for ( size_t i = 0; i < p->element_count; ++i ) {
        p->elements[i] = calloc(hash_len, sizeof **p->elements);
        size_t j = (leaf_idx >> i) ^ 1u;
        memcpy(p->elements[i], tiers[i][j], hash_len);
    }

    p->elements[p->element_count] =
        calloc(hash_len, sizeof **p->elements);

    memcpy(
        p->elements[p->element_count],
        tiers[p->element_count][0],
        hash_len
   );
   ++(p->element_count);

    if ( tiers ) { free_materialization(tiers, mt->leaf_count, hash_len); }
    return p;
}

bool
is_valid_proof (struct mproof * mp) {

    if ( !mp ) { return false; }

    unsigned char * leaf_hash = NULL;
    switch ( mp->t ) {
        case of_inclusion: {
            unsigned char * buf = calloc(1, mp->msg_sz + 1);
            memcpy(buf + 1, mp->msg, mp->msg_sz);
            leaf_hash = calloc(1, mp->hash_sz);
            get_hash(buf, mp->msg_sz + 1, leaf_hash, mp->hash_sz);
        } break;

        case of_knowledge:
            leaf_hash = mp->msg;
            break;
    }

    unsigned char * intermed_hash = calloc(1, mp->hash_sz);
    for ( size_t i = 0; i < mp->element_count - 1; ++i ) {
        unsigned char * intermed_preimage = calloc(
            mp->hash_sz * 2 + 1,
            sizeof (*intermed_preimage)
        );
        unsigned char * cptr = intermed_preimage;
        unsigned char height = i + 1;
        memcpy(cptr, &height, 1);
        cptr++;
        unsigned char * ancestral_hash = height == 1 ? leaf_hash : intermed_hash;
        if ( memcmp(ancestral_hash, mp->elements[i], mp->hash_sz) < 1 ) {
            memcpy(cptr, ancestral_hash, mp->hash_sz);
            cptr += mp->hash_sz;
            memcpy(cptr, mp->elements[i], mp->hash_sz);
        } else {
            memcpy(cptr, mp->elements[i], mp->hash_sz);
            cptr += mp->hash_sz;
            memcpy(cptr, ancestral_hash, mp->hash_sz);
        }
        get_hash(intermed_preimage, mp->hash_sz * 2 + 1, intermed_hash, mp->hash_sz);
        crypto_wipe(intermed_preimage, mp->hash_sz * 2 + 1);
        free(intermed_preimage);
    }

    bool valid = !memcmp(intermed_hash, mp->elements[mp->element_count - 1], mp->hash_sz);
    crypto_wipe(intermed_hash, mp->hash_sz);
    free(intermed_hash);

    return valid;
}

void
free_proof (struct mproof * mp) {

    if ( !mp ) { return; }

    for ( size_t i = 0; i < mp->element_count; ++i ) {
        crypto_wipe(mp->elements[i], mp->hash_sz);
        free(mp->elements[i]);
    }
    crypto_wipe(mp->elements, mp->element_count * sizeof (*mp->elements));
    free(mp->elements);
    crypto_wipe(mp, sizeof *mp);
    free(mp);
}
