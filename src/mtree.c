#include <mtree.h>

struct mtree *
create_mt (
    size_t msg_count,
    enum msg_type msg_src [],
    char * msg_loc [],
    size_t hash_len
) {

    if ( msg_count == 0 || hash_len < 1 || hash_len > 64 ) {
        return NULL;
    }

    struct mtree * mt = calloc(1, sizeof *mt);
    mt->hash_sz = hash_len;
    mt->leaf_count = msg_count;
    mt->leaves = calloc(mt->leaf_count, sizeof *mt->leaves);
    for ( size_t i = 0; i < mt->leaf_count; ++i ) {
        mt->leaves[i].t = msg_src[i];
        switch ( msg_src[i] ) {
            case from_file: {
                signed sz = file_to_buf(msg_loc[i], &mt->leaves[i].data);
                if ( sz < 0 ) {
                    // fail out
                }
                mt->leaves[i].sz = sz;
                mt->leaves[i].location = strdup(msg_loc[i]);
            } break;

            case from_str:
                mt->leaves[i].location = NULL;
                mt->leaves[i].sz = strlen(msg_loc[i]) + 1;
                mt->leaves[i].data = (unsigned char * )strdup(msg_loc[i]);
                break;

            case decoy:
                mt->leaves[i].location = NULL;
                mt->leaves[i].sz = 64;
                mt->leaves[i].data = calloc(64, sizeof *mt->leaves[i].data);
                getrandom(mt->leaves[i].data, 64, 0);
                break;
        }
    }

    return mt;
}

// todo: replace free() with crypto_wipe()
void
free_mt (struct mtree * mt) {

    if ( !mt ) { return; }

    for ( size_t i = 0; i < mt->leaf_count; ++i ) {
        switch ( mt->leaves[i].t ) {
            case from_file: free(mt->leaves[i].location); // fallthrough
            case from_str: // fallthrough
            case decoy: free(mt->leaves[i].data); break;
        }
    }

    free(mt->leaves);
    free(mt);
}

unsigned char ***
materialize_tree (const struct mtree * mt, size_t sz_override) {

    if ( !mt || sz_override > 64 ) {
        return NULL;
    }

    size_t len = sz_override ? sz_override : mt->hash_sz;
    unsigned char * rt = calloc(len, sizeof *rt);

    size_t n_tiers = log2(mt->leaf_count) + 1;
    unsigned char *** tiers = calloc(n_tiers, sizeof (*tiers));
    for ( size_t i = 0; i < n_tiers; ++i ) {
        size_t n_nodes = mt->leaf_count >> i;
        tiers[i] = calloc(n_nodes, sizeof (**tiers));
        for ( size_t j = 0; j < n_nodes; ++j ) {
            unsigned char * b = calloc(len, sizeof(***tiers));
            if ( i == 0 ) { // bottom-tier contains hashes of messages
                // add 0x00 prefix
                unsigned char * buf = calloc(1, mt->leaves[j].sz + 1);
                memcpy(buf + 1, mt->leaves[j].data, mt->leaves[j].sz);
                get_hash(buf, mt->leaves[j].sz + 1, b, len);
                free(buf);
            } else {
                unsigned char * comb = calloc(len * 2 + 1, sizeof(***tiers));
                unsigned char * cptr = comb;
                unsigned char height = i;
                memcpy(cptr, &height, 1);
                cptr++;
                // todo: replace memcmp() with with crypto_verifyN()
                if ( memcmp(tiers[i-1][j*2], tiers[i-1][j*2+1], len) < 1 ) {
                    memcpy(cptr, tiers[i-1][j*2], len);
                    cptr += len;
                    memcpy(cptr, tiers[i-1][j*2+1], len);
                } else {
                    memcpy(cptr, tiers[i-1][j*2+1], len);
                    cptr += len;
                    memcpy(cptr, tiers[i-1][j*2], len);
                }
                get_hash(comb, len * 2 + 1, b, len);
            }

            tiers[i][j] = b;
        }
    }

    return tiers;
}

void
free_materialization (unsigned char *** tiers, size_t leaf_count) {

    size_t n_tiers = log2(leaf_count) + 1;
    for ( size_t i = 0; i < n_tiers; ++i ) {
        size_t n_nodes = leaf_count >> i;
        for ( size_t j = 0; j < n_nodes; ++j ) {
            free(tiers[i][j]);
        }
        free(tiers[i]);
    }
    free(tiers);
}

unsigned char *
root_from_tree (const struct mtree * mt, size_t sz_override) {

    if ( !mt || sz_override > 64 ) {
        return NULL;
    }

    size_t len = sz_override ? sz_override : mt->hash_sz;
    unsigned char * rt = calloc(len, sizeof *rt);

    unsigned char *** tiers = materialize_tree(mt, sz_override);

    size_t n_tiers = log2(mt->leaf_count);
    memcpy(rt, tiers[n_tiers][0], len);
    free_materialization(tiers, mt->leaf_count);

    return rt;
}

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
                unsigned char * comb = calloc(hash_len * 4 + 1, sizeof(***tiers));
                unsigned char height = i;
                memcpy(comb, &height, 1);
                if ( strcmp(tiers[i-1][j*2], tiers[i-1][j*2+1]) < 1 ) {
                    memcpy(comb + 1, tiers[i-1][j*2], hash_len * 2);
                    memcpy(comb + hash_len * 2 + 1, tiers[i-1][j*2+1], hash_len * 2);
                } else {
                    memcpy(comb + 1, tiers[i-1][j*2+1], hash_len * 2);
                    memcpy(comb + hash_len * 2 + 1, tiers[i-1][j*2], hash_len * 2);
                }
                get_hash(comb, hash_len * 4 + 1, b, hash_len);
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
    size_t sz,
    const unsigned char * sibs [],
    size_t sib_count
) {

    size_t hash_len = strlen(rt) / 2;

    unsigned char * leaf = calloc(hash_len, sizeof(*leaf));
    get_hash(msg, sz, leaf, hash_len);

    unsigned char * leafhx = to_hex(leaf, hash_len);
    free(leaf);

    unsigned char * comb = calloc(hash_len * 4 + 1, sizeof(*leaf));
    for ( size_t i = 0; i < sib_count; ++i ) {
        unsigned char height = i + 1;
        memcpy(comb, &height, 1);
        if ( strcmp(sibs[i], leafhx) < 1 ) {
            memcpy(comb + 1, sibs[i], hash_len * 2);
            memcpy(comb + hash_len * 2 + 1, leafhx, hash_len * 2);
        } else {
            memcpy(comb + 1, leafhx, hash_len * 2);
            memcpy(comb + hash_len * 2 + 1, sibs[i], hash_len * 2);
        }

        free(leafhx);
        get_hash(comb, hash_len * 4 + 1, leaf, hash_len);
        leafhx = to_hex(leaf, hash_len);
    }

    for ( size_t i = 0; i < hash_len * 2; ++i ) {
        if ( rt[i] != leafhx[i] ) {
            return 0;
        }
    }

    return 1;
}

