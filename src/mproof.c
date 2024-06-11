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

        case of_knowledge:
            p->msg_sz = hash_len;
            p->msg = calloc(1, hash_len);
            get_hash(leaf.data, leaf.sz, p->msg, hash_len);
            break;
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

    if ( tiers ) { free_materialization(tiers, mt->leaf_count); }
    return p;
}

void
free_proof (struct mproof * mp) {

    if ( !mp ) { return; }

    for ( size_t i = 0; i < mp->element_count; ++i ) {
        free(mp->elements[i]);
    }
    free(mp->elements);
    free(mp);
}
