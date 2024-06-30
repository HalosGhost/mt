#include <mt-stat.h>

signed
main (signed argc, char * argv []) {

    enum out_fmt fmt = FMT(list);
    signed status = 0;

    struct textenc * enc = NULL;
    struct mtree * mt = NULL;
    unsigned char * stored_rt = NULL;
    unsigned char * hx = NULL;

    signed oi = 0, c = 0;
    while ( (c = getopt_long(argc, argv, optstr, os, &oi)) != -1 ) {
        switch ( c ) {
            case 'f':
                FOR_EACH_OUTFMT(HANDLE_FMT_MATCH)
                /* else */ {
                    fprintf(stderr, "Unrecognized format: %s\n", optarg);
                    status = 1;
                    goto cleanup;
                } break;

            default:
                status = 1;
                // fallthrough
            case 'h':
                stat_usage(argv[0]);
                goto cleanup;
        }
    }

    if ( optind >= argc ) {
        fputs("Exactly one Merkle Tree must be specified.\n", stderr);
        status = 1;
        goto cleanup;
    }

    FILE * r = fopen(argv[optind], "r");
    enc = fr_txtenc(r);
    fclose(r);
    if ( !enc ) {
        fprintf(stderr, "Failed to read textual encoding from %s\n", argv[optind]);
        status = 1;
        goto cleanup;
    }

    mt = decode_mt(enc, &stored_rt);
    if ( !mt ) {
        fputs("Failed to decode tree\n", stderr);
        status = 1;
        goto cleanup;
    }

    // read mt and stored_rt from argv[optind]
    hx = to_hex(stored_rt, mt->hash_sz);

    // check integrity
    fprintf(stderr, "[%.8s] integrity", hx);
    unsigned char * rt = root_from_tree(mt, 0);
    bool integ = !memcmp(stored_rt, rt, mt->hash_sz);
    fprintf(stderr, "\t [%s]\n", integ ? "PASS" : "FAIL");

    switch ( fmt ) {
        case FMT(none):                  break;
        case FMT(list): list_leaves(mt); break;
        case FMT(tree): draw_tree(mt);   break;
    }

    cleanup:
        return status;
}

void
list_leaves (const struct mtree * mt) {

    unsigned char *** tiers = materialize_tree(mt, mt->hash_sz);

    signed max_leaf_idx_digits = snprintf(NULL, 0, "%zu", mt->leaf_count - 1);
    for ( size_t i = 0; i < mt->leaf_count; ++i ) {
        unsigned char * hx = to_hex(tiers[0][i], mt->hash_sz);
        struct mt_msg leaf = mt->leaves[i];
        printf("[%*zu]\t%s\t%s\t%s\n", max_leaf_idx_digits, i,
            msg_type_name[leaf.t],
            leaf.location,
            hx
        );
        crypto_wipe(hx, mt->hash_sz * 2);
        free(hx);
    }

    if ( tiers ) { free_materialization(tiers, mt->leaf_count, mt->hash_sz); }
}

void
draw_tree (const struct mtree * mt) {

    unsigned char *** tiers = materialize_tree(mt, mt->hash_sz);
    size_t n_tiers = log2(mt->leaf_count) + 1;

    uint8_t * counter = calloc(n_tiers, sizeof (*counter));
    signed max_leaf_idx_digits = snprintf(NULL, 0, "%zu", mt->leaf_count - 1);
    for ( size_t l = 0; l < mt->leaf_count; ++l ) {
        for ( size_t n = n_tiers; n > 0; --n ) {
            size_t i = l >> (n - 1);
            size_t indent = n_tiers - n;
            if ( counter[n - 1] <= i ) {
                for ( size_t d = 1; d < indent; ++d ) {
                    printf("%s  ", (i >> (indent - d)) & 1 ? " " : "│");
                }
                if ( indent ) {
                    printf("%s ", i & 1 ? "└─" : "├─");
                }
                unsigned char * hx = to_hex(tiers[n - 1][i], mt->hash_sz);
                if ( n == 1 ) {
                    printf("[%*zu] ", max_leaf_idx_digits, i);
                }
                printf("%s", hx);
                crypto_wipe(hx, mt->hash_sz * 2);
                free(hx);
                putchar('\n');

                ++counter[n - 1];
            }
        }
    }

    if ( tiers ) { free_materialization(tiers, mt->leaf_count, mt->hash_sz); }
}

_Noreturn void
stat_usage (const char * prog) {

    static const char header [] =
        "Usage: %s [options] <path/to/mtree>\n"
        "%s -- Inspect Merkle Trees\n\n"
    ;

    fprintf(stderr, header, prog, prog);

    signed longest = 0;
    #define GET_LONGEST_FLAG_COL(_l, _s, _h) do { \
        signed i = snprintf(NULL, 0, "  -%s, --%s", (_s), (_l)); \
        longest = (i > longest) ? i : longest; \
    } while ( false );

    #define GET_LONGEST_OPTION_COL(_l, _a, _s, _h) do { \
        signed i = snprintf(NULL, 0, "  -%s, --%s=<%s>", (_s), (_l), (_a)); \
        longest = (i > longest) ? i : longest; \
    } while ( false );

    FOR_EACH_FLAG(GET_LONGEST_FLAG_COL)
    FOR_EACH_OPTION(GET_LONGEST_OPTION_COL)

    #define PRINT_FLAG_HELP(_l, _s, _h) do { \
        signed i = fprintf(stderr, "  -%s, --%s", (_s), (_l)); \
        fprintf(stderr, "%-*s\t%s\n", longest - i, " ", (_h)); \
    } while ( false );

    fprintf(stderr, "Flags:\n");
    FOR_EACH_FLAG(PRINT_FLAG_HELP)

    #define PRINT_OPTION_HELP(_l, _a, _s, _h) do { \
        signed i = fprintf(stderr, "  -%s, --%s=<%s>", (_s), (_l), (_a)); \
        fprintf(stderr, "%-*s\t%s\n", longest - i, " ", (_h)); \
    } while ( false );

    fprintf(stderr, "\nOptions:\n");
    FOR_EACH_OPTION(PRINT_OPTION_HELP)

    exit(EXIT_SUCCESS);
}

