#include <mt-verify.h>

signed
main (signed argc, char * argv []) {

    signed status = EXIT_SUCCESS;

    unsigned char * rt = NULL;

    struct mproof * p = NULL;
    struct textenc * prf_enc = NULL;

    unsigned char * rt_hex = NULL;
    struct textenc * rt_enc = NULL;
    char * rt_p = NULL;

    char * out_p = NULL;
    size_t out_p_len = 0;

    signed oi = 0, c = 0;
    while ( (c = getopt_long(argc, argv, optstr, os, &oi)) != -1 ) {
        switch ( c ) {
            case 'r': // read hex
                if ( optarg ) {
                    rt_hex = (unsigned char * )strdup(optarg);
                }
                break;

            case 'R': // read textual-encoding
                if ( optarg ) {
                    rt_p = strdup(optarg);
                }
                break;

            case 'o':
                if ( optarg ) {
                    out_p = strdup(optarg);
                    out_p_len = strlen(out_p);
                }
                break;

            default:
                status = 1;
                // fallthrough
            case 'h':
                verify_usage(argv[0]);
                goto cleanup;
        }
    }

    if ( optind >= argc ) {
        fputs("No Merkle Proof provided.\n", stderr);
        status = 1;
        goto cleanup;
    }

    FILE * r = fopen(argv[optind], "r");
    prf_enc = fr_txtenc(r);
    fclose(r);
    if ( !prf_enc ) {
        fprintf(stderr, "Failed to read textual encoding from %s\n", argv[optind]);
        status = 1;
        goto cleanup;
    }

    p = decode_mp(prf_enc);

    // should we prefer one over the other? verify they match?
    if ( rt_p ) {
        FILE * f = fopen(rt_p, "r");
        rt_enc = fr_txtenc(f);
        fclose(f);
        size_t N = 0;
        rt = decode_mr(rt_enc, &N);
        if ( N != p->hash_sz ) {
            fputs("[ERROR]: Roots are of different sizes!\n", stderr);
            status = 1;
            goto cleanup;
        }
    } else if ( rt_hex ) {
        rt = from_hex(rt_hex, p->hash_sz * 2);
    }

    if ( !rt ) {
        fputs("[WARNING]: No explicit root provided!\n", stderr);
    } else if ( memcmp(rt, p->elements[p->element_count - 1], p->hash_sz) ) {
        fputs("[ERROR]: Mismatched root!\n", stderr);
        status = 1;
        goto cleanup;
    }

    if ( is_valid_proof(p) ) {
        fputs("Proof verified!\n", stderr);
        if ( out_p ) {
            if ( p->t != of_inclusion ) {
                fputs("[WARNING]: Cannot write message to file for knowlege-proofs\n", stderr);
            } else if ( out_p_len == 1 && !strncmp(out_p, "-", 1) ) {
                printf("%s", p->msg);
                fputs("wrote file (-)\n", stderr);
            } else {
                FILE * f = fopen(out_p, "w");
                fprintf(f, "%s", p->msg);
                fclose(f);
                fprintf(stderr, "wrote file (%s)\n", out_p);
            }
        }
    } else {
        fprintf(stderr, "Proof Invalid!\n");
        status = 1;
    }

    cleanup:
        if ( rt ) { free(rt); }
        if ( rt_hex ) { free(rt_hex); }
        if ( p ) { free_proof(p); }
        if ( prf_enc ) { free_txtenc(prf_enc); }
        return status;
}

_Noreturn void
verify_usage (const char * prog) {

    static const char header [] =
        "Usage: %s [options] <path/to/proof>\n"
        "%s -- Verify a Merkle Path\n\n"
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

