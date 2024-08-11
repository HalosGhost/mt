#include <mt-prove.h>

signed
main (signed argc, char * argv []) {

    signed status = 0;

    struct textenc * mt_enc = NULL;
    struct mtree * mt = NULL;
    unsigned char * rt = NULL;

    bool find_proof = false;
    enum proof_type type = of_inclusion;
    size_t leaf_idx = 0;
    size_t hash_len_override = 0;
    bool no_prf_no_path = false;

    struct mproof * p = NULL;
    struct textenc * prf_enc = NULL;
    char * prf_p = NULL;
    char * prf_path = NULL;
    size_t prf_path_len = 0;
    bool p_is_stdout = false;

    enum rt_encoding { as_hex, as_pem } encoding = as_hex;
    unsigned char * rt_hex = NULL;
    size_t rt_hex_len = 0;
    struct textenc * rt_enc = NULL;
    char * rt_p = NULL;
    size_t rt_path_len = 0;
    bool r_is_stdout = false;

    signed oi = 0, c = 0;
    while ( (c = getopt_long(argc, argv, optstr, os, &oi)) != -1 ) {
        switch ( c ) {
            case 'K':
                if ( optarg ) {
                    find_proof = true;
                    type = of_knowledge;
                    sscanf(optarg, "%zu", &leaf_idx);
                } break;

            case 'I':
                if ( optarg ) {
                    find_proof = true;
                    type = of_inclusion;
                    sscanf(optarg, "%zu", &leaf_idx);
                } break;

            case 'b': if ( optarg ) {
                sscanf(optarg, "%zu", &hash_len_override);
                if ( hash_len_override > 64 ) {
                    fprintf(stderr, "Invalid size (%zu), using the native size\n", hash_len_override);
                    hash_len_override = 0;
                }
            } break;

            case 'p':
                if ( optarg ) {
                    prf_p = strdup(optarg);
                    prf_path_len = strlen(prf_p);
                } break;

            case 'r':
                if ( optarg ) {
                    encoding = as_hex;
                    rt_p = strdup(optarg);
                    rt_path_len = strlen(rt_p);
                } break;

            case 'R':
                if ( optarg ) {
                    encoding = as_pem;
                    rt_p = strdup(optarg);
                    rt_path_len = strlen(rt_p);
                } break;

            default:
                status = 1;
                // fallthrough
            case 'h':
                prove_usage(argv[0]);
                goto cleanup;
        }
    }

    if ( optind >= argc ) {
        fputs("Exactly one Merkle Tree must be specified.\n", stderr);
        status = 1;
        goto cleanup;
    }

    FILE * r = fopen(argv[optind], "r");
    mt_enc = fr_txtenc(r);
    fclose(r);
    if ( !mt_enc ) {
        fprintf(stderr, "Failed to read textual encoding from %s\n", argv[optind]);
        status = 1;
        goto cleanup;
    }

    mt = decode_mt(mt_enc, NULL);
    if ( !mt ) {
        fputs("Failed to decode tree\n", stderr);
        status = 1;
        goto cleanup;
    }

    if ( leaf_idx >= mt->leaf_count ) {
        fputs("Leaf index out-of-range\n", stderr);
        status = 1;
        goto cleanup;
    }

    signed written = 0;

    no_prf_no_path = !find_proof && !rt_p;
    if ( rt_p || no_prf_no_path ) {
        rt = root_from_tree(mt, hash_len_override);
        size_t actual_sz = !hash_len_override ? mt->hash_sz : hash_len_override;
        rt_hex = to_hex(rt, actual_sz);
        rt_hex_len = actual_sz * 2;
        rt_enc = encode_mr(rt, actual_sz);
        r_is_stdout = rt_p && strlen(rt_p) == 1 && !strncmp(rt_p, "-", 1);
        if ( r_is_stdout || no_prf_no_path ) {
            char * buf = NULL;
            size_t buf_sz = 0;
            FILE * w = open_memstream(&buf, &buf_sz);

            switch ( encoding ) {
                case as_hex: written = fprintf(w, "%s\n", rt_hex); break;
                case as_pem: written = fw_txtenc(w, rt_enc); break;
            }
            fclose(w);

            if ( written > 0 ) {
                printf("%s", buf);
            }

            crypto_wipe(buf, buf_sz);
            free(buf);
        } else if ( rt_p ) {
            FILE * f = fopen(rt_p, "w");
            switch ( encoding ) {
                case as_hex: written = fprintf(f, "%s\n", rt_hex); break;
                case as_pem: written = fw_txtenc(f, rt_enc); break;
            }
            fclose(f);
        }

        if ( written < 0 ) {
            fprintf(stderr, "failed to write root to %s\n", rt_p);
        } else {
            fprintf(stderr, "wrote root (%s)\n", rt_p ? rt_p : "-");
        }
        fflush(stderr);
    }

    if ( find_proof ) {
        written = 0;
        p = proof_from_tree(mt, leaf_idx, hash_len_override, type);

        prf_enc = encode_mp(p);

        p_is_stdout = prf_p && strlen(prf_p) == 1 && !strncmp(prf_p, "-", 1);
        if ( p_is_stdout || !prf_p ) {
            char * buf = NULL;
            size_t buf_sz = 0;
            FILE * w = open_memstream(&buf, &buf_sz);

            written = fw_txtenc(w, prf_enc);
            fclose(w);

            if ( written > 0 ) {
                printf("%s", buf);
            }
            prf_path = "-";

            crypto_wipe(buf, buf_sz);
            free(buf);
        } else if ( prf_p ) {
            FILE * f = fopen(prf_p, "w");
            written = fw_txtenc(f, prf_enc);
            prf_path = prf_p;
            fclose(f);
        }

        if ( written < 0 ) {
            fprintf(stderr, "failed to write proof to %s\n", prf_path);
        } else {
            fprintf(stderr, "wrote proof (%s)\n", prf_path);
        }
        fflush(stderr);
    }

    cleanup:
        if ( mt_enc ) { free_txtenc(mt_enc); }
        if ( prf_enc ) { free_txtenc(prf_enc); }
        if ( rt_hex ) { crypto_wipe(rt_hex, rt_hex_len); free(rt_hex); }
        if ( rt_enc ) { free_txtenc(rt_enc); }
        if ( rt ) {
            crypto_wipe(
                rt,
                hash_len_override ? hash_len_override : mt->hash_sz
            );
            free(rt);
        }
        if ( mt ) { free_mt(mt); }
        if ( p ) { free_proof(p); }
        if ( !p_is_stdout && prf_p ) {
            crypto_wipe(prf_p, prf_path_len);
            free(prf_p);
        }
        if ( !r_is_stdout && rt_p ) {
            crypto_wipe(rt_p, rt_path_len);
            free(rt_p);
        }
        return status;
}

_Noreturn void
prove_usage (const char * prog) {

    static const char header [] =
        "Usage: %s [options] <path/to/tree>\n"
        "%s -- Prove properties of messages in Merkle Trees\n\n"
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
        fprintf(stderr, "%-*s  \t%s\n", longest - i, " ", (_h)); \
    } while ( false );

    fprintf(stderr, "Flags:\n");
    FOR_EACH_FLAG(PRINT_FLAG_HELP)

    #define PRINT_OPTION_HELP(_l, _a, _s, _h) do { \
        signed i = fprintf(stderr, "  -%s, --%s=<%s>", (_s), (_l), (_a)); \
        fprintf(stderr, "%-*s  \t%s\n", longest - i, " ", (_h)); \
    } while ( false );

    fprintf(stderr, "\nOptions:\n");
    FOR_EACH_OPTION(PRINT_OPTION_HELP)

    exit(EXIT_SUCCESS);
}

