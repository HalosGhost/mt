#include <mt-create.h>

signed
main (signed argc, char * argv []) {

    size_t hash_len = 64;
    char * out_p = NULL;
    char ** msg_loc = NULL;
    enum msg_type * msg_src = NULL;
    size_t msg_count = 0;
    size_t leaf_count = 0;
    struct mtree * mt = NULL;
    bool avoid_ppi_leakage = true;

    struct textenc * enc = NULL;

    signed oi = 0, c = 0;
    while ( (c = getopt_long(argc, argv, optstr, os, &oi)) != -1 ) {
        switch ( c ) {
            case 'b':
                if ( optarg ) { sscanf(optarg, "%zu", &hash_len); }
                if ( hash_len < 1 || hash_len > 64 ) {
                    fprintf(stderr, "%zu out-of-range; defaulting to 64\n", hash_len);
                    hash_len = 64;
                } break;

            case 'd':
                ++msg_count;
                msg_loc = realloc(msg_loc, msg_count * sizeof *msg_loc);
                msg_src = realloc(msg_src, msg_count * sizeof *msg_src);
                msg_loc[msg_count - 1] = NULL;
                msg_src[msg_count - 1] = decoy;
                break;

            case 'x': avoid_ppi_leakage = false; break;

            case 's':
                if ( optarg ) {
                    ++msg_count;
                    msg_loc = realloc(msg_loc, msg_count * sizeof *msg_loc);
                    msg_src = realloc(msg_src, msg_count * sizeof *msg_src);
                    msg_loc[msg_count - 1] = strdup(optarg);
                    msg_src[msg_count - 1] = from_str;
                } break;

            case 'f':
                if ( optarg ) {
                    ++msg_count;
                    msg_loc = realloc(msg_loc, msg_count * sizeof *msg_loc);
                    msg_src = realloc(msg_src, msg_count * sizeof *msg_src);
                    msg_loc[msg_count - 1] = strdup(optarg);
                    msg_src[msg_count - 1] = from_file;
                } break;

            case 'o':
                if ( optarg ) {
                    out_p = strdup(optarg);
                } break;

            case 'l':
                if ( optarg ) {
                    sscanf(optarg, "%zu", &leaf_count);
                } break;

            default:
                create_usage(argv[0]);
                break;
        }
    }

    if ( msg_count < 1 ) {
        fputs("At least one message must be specified.\n", stderr);
        goto cleanup;
    }

    { // guarantee power-of-two leaves, and L >= 2M
        if ( !leaf_count && avoid_ppi_leakage ) {
            leaf_count = is_pow2(msg_count)
                       ? msg_count * 2
                       : next_pow2(msg_count * 2);
        } else if ( !leaf_count && !is_pow2(msg_count) ) {
            leaf_count = next_pow2(msg_count);
        } else if ( !avoid_ppi_leakage ) {
            leaf_count = msg_count;
        }

        if ( leaf_count ) {
            if ( msg_count > leaf_count ) {
                fprintf(stderr, "More messages provided than requested leaf-count. Exiting.\n");
                goto cleanup;
            }

            if ( !is_pow2(leaf_count) ) {
                fprintf(stderr, "Requested leaf-count is non-power-of-two; rounding up.\n");
                leaf_count = next_pow2(leaf_count);
            }

            size_t diff = msg_count;
            while ( diff < leaf_count ) {
                diff++;
                msg_loc = realloc(msg_loc, diff * sizeof *msg_loc);
                msg_src = realloc(msg_src, diff * sizeof *msg_src);
                msg_loc[diff - 1] = NULL;
                msg_src[diff - 1] = decoy;
            }
        }
    }

    // avoids prior-preimage-inclusion leakage
    if ( avoid_ppi_leakage ) {
        for ( size_t i = 1; i < msg_count; i += 2 ) {
            char * tmp_loc = msg_loc[i];
            enum msg_type tmp_src = msg_src[i];

            msg_loc[i] = msg_loc[leaf_count - i - 1];
            msg_src[i] = msg_src[leaf_count - i - 1];

            msg_loc[leaf_count - i - 1] = tmp_loc;
            msg_src[leaf_count - i - 1] = tmp_src;
        }
    }

    mt = create_mt(leaf_count, msg_src, msg_loc, hash_len);

    unsigned char * rt = root_from_tree(mt, 0);
    unsigned char * hx = to_hex(rt, mt->hash_sz);
    signed written = 0;
    char * path = NULL;

    enc = encode_mt(mt);
    if ( out_p && strlen(out_p) == 1 && strncmp(out_p, "-", 1) == 0 ) {
        char * buf = NULL;
        size_t buf_sz = 0;
        FILE * w = open_memstream(&buf, &buf_sz);

        written = fw_txtenc(w, enc);
        fclose(w);

        if ( written > 0 ) {
            printf("%s", buf);
        }
        path = "-";

        free(buf);
    } else if ( out_p ) {
        FILE * f = fopen(out_p, "w");
        written = fw_txtenc(f, enc);
        path = out_p;
        fclose(f);
    } else {
        size_t p_sz = 0;
        FILE * p = open_memstream(&path, &p_sz);
        fprintf(p, "%.8s.mt", hx);
        fclose(p);
        FILE * f = fopen(path, "w");
        written = fw_txtenc(f, enc);
        fclose(f);
    }

    if ( written < 0 ) {
        fprintf(stderr, "[%.8s]: failed to write textual encoding to %s\n", hx, path);
    } else {
        fprintf(stderr, "[%.8s]: created (%s)\n", hx, path);
    }
    fflush(stderr);

    if ( !out_p ) {
        free(path);
    }

    free(hx);
    free(rt);

    cleanup:
        if ( enc ) {
            free_txtenc(enc);
        }
        if ( leaf_count ) {
            for ( size_t i = 0; i < msg_count; ++i ) {
                if ( msg_loc[i] ) { free(msg_loc[i]); }
            }
            if ( msg_src ) { free(msg_src); }
        }
        if ( mt ) { free_mt(mt); }
        return EXIT_SUCCESS;
}

_Noreturn void
create_usage (const char * prog) {

    static const char header [] =
        "Usage: %s [options]\n"
        "%s -- Create Merkle Trees\n\n"
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

size_t
next_pow2 (size_t v) {

    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    v++;    

    return v;
}

