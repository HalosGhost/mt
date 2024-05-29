#include <mtree.h>

// for the moment, this operates on hash-hex instead of plain bytes
// this is less efficient, but enables by-hand verification
unsigned char *
get_merkle_root (
    size_t msg_count,
    unsigned char * messages [],
    size_t msg_length[],
    size_t hash_len
) {

    unsigned char * rt = calloc(hash_len * 2, sizeof(*rt));

    size_t n_tiers = log2(msg_count) + 1;
    unsigned char *** tiers = calloc(n_tiers, sizeof (*tiers));

    // walk up each tier building a hash from two in the previous
    for ( size_t i = 0; i < n_tiers; ++i ) {
        size_t n_nodes = msg_count >> i;
        tiers[i] = calloc(n_nodes, sizeof (**tiers));
        for ( size_t j = 0; j < n_nodes; ++j ) {
            unsigned char * b = calloc(hash_len, sizeof(***tiers));
            if ( i == 0 ) { // bottom-tier contains hashes of messages
                get_hash(messages[j], msg_length[j], b, hash_len);
            } else {
                unsigned char * comb = calloc(hash_len * 4, sizeof(***tiers));
                memcpy(comb, tiers[i-1][j*2], hash_len * 2);
                memcpy(comb + hash_len * 2, tiers[i-1][j*2+1], hash_len * 2);
                get_hash(comb, hash_len * 4, b, hash_len);
            }

            tiers[i][j] = to_hex(b, hash_len);
            fprintf(stderr, "(%zu, %zu) %s\n", i, j, tiers[i][j]);
            free(b);
        }
    }

    memcpy(rt, tiers[n_tiers - 1][0], hash_len * 2);
    for ( size_t i = 0; i < n_tiers; ++i ) {
        size_t n_nodes = msg_count >> i;
        for ( size_t j = 0; j < n_nodes; ++j ) {
            free(tiers[i][j]);
        }
        free(tiers[i]);
    }
    free(tiers);

    return rt;
}

bool
verify_merkle_path (
    const unsigned char * rt,
    const unsigned char * msg,
    const struct sibling * sibs,
    size_t sib_count
) {

    size_t hash_len = strlen(rt) / 2;

    unsigned char * leaf = calloc(hash_len, sizeof(*leaf));
    get_hash(msg, strlen(msg), leaf, hash_len);

    unsigned char * leafhx = to_hex(leaf, hash_len);
    free(leaf);

    unsigned char * comb = calloc(hash_len * 4, sizeof(*leaf));
    for ( size_t i = 0; i < sib_count; ++i ) {
        if ( sibs[i].dir == left ) {
            memcpy(comb, sibs[i].digest, hash_len * 2);
            memcpy(comb + hash_len * 2, leafhx, hash_len * 2);
        } else {
            memcpy(comb, leafhx, hash_len * 2);
            memcpy(comb + hash_len * 2, sibs[i].digest, hash_len * 2);
        }

        free(leafhx);
        get_hash(comb, hash_len * 4, leaf, hash_len);
        leafhx = to_hex(leaf, hash_len);
    }

    for ( size_t i = 0; i < hash_len * 2; ++i ) {
        if ( rt[i] != leafhx[i] ) {
            return 0;
        }
    }

    return 1;
}

