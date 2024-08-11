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
                randombytes(mt->leaves[i].data, 64);
                break;
        }
    }

    return mt;
}

void
free_mt (struct mtree * mt) {

    if ( !mt ) { return; }

    for ( size_t i = 0; i < mt->leaf_count; ++i ) {
        switch ( mt->leaves[i].t ) {
            case from_file:
                crypto_wipe(
                    mt->leaves[i].location,
                    strlen(mt->leaves[i].location)
                );
                free(mt->leaves[i].location);
                // fallthrough
            case from_str: // fallthrough
            case decoy:
                crypto_wipe(mt->leaves[i].data, mt->leaves[i].sz);
                free(mt->leaves[i].data);
                break;
        }
    }

    crypto_wipe(mt->leaves, mt->leaf_count * sizeof (*mt->leaves));
    free(mt->leaves);
    crypto_wipe(mt, sizeof (*mt));
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
                crypto_wipe(buf, mt->leaves[j].sz + 1);
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
free_materialization (unsigned char *** tiers, size_t leaf_count, size_t sz) {

    size_t n_tiers = log2(leaf_count) + 1;
    for ( size_t i = 0; i < n_tiers; ++i ) {
        size_t n_nodes = leaf_count >> i;
        for ( size_t j = 0; j < n_nodes; ++j ) {
            crypto_wipe(tiers[i][j], sz);
            free(tiers[i][j]);
        }
        crypto_wipe(tiers[i], n_nodes * sizeof (*tiers[i]));
        free(tiers[i]);
    }
    crypto_wipe(tiers, n_tiers * sizeof (*tiers));
    free(tiers);
}

unsigned char *
root_from_tree (const struct mtree * mt, size_t sz_override) {

    if ( !mt || sz_override > 64 ) {
        return NULL;
    }

    size_t len = sz_override ? sz_override : mt->hash_sz;
    unsigned char * rt = calloc(len, sizeof *rt);

    unsigned char *** tiers = materialize_tree(mt, len);

    size_t n_tiers = log2(mt->leaf_count);
    memcpy(rt, tiers[n_tiers][0], len);
    free_materialization(tiers, mt->leaf_count, len);

    return rt;
}

