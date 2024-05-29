#include <mt-verify.h>

signed
main (signed argc, char * argv []) {

    unsigned char * rt = NULL;
    unsigned char * msg = NULL;
    struct sibling * siblings = NULL;
    size_t sibling_count = 0;

    signed oi = 0, c = 0;
    while ( (c = getopt_long(argc, argv, optstr, os, &oi)) != -1 ) {
        switch ( c ) {
            case 'R':
                if ( optarg ) {
                    rt = strdup(optarg);
                }
                break;

            case 's':
                if ( optarg ) {
                    if ( msg ) {
                        fputs("Cannot verify multiple messages at once.\n", stderr);
                        goto cleanup;
                    }
                    msg = strdup(optarg);
                }
                break;

            case 'f':
                if ( optarg ) {
                    if ( msg ) {
                        fputs("Cannot verify multiple messages at once.\n", stderr);
                        goto cleanup;
                    }
                    file2buf(optarg, &msg);
                }
                break;

            case 'l': {
                ++sibling_count;
                siblings = realloc(siblings, sibling_count * sizeof(struct sibling));
                siblings[sibling_count - 1].dir = left;
                siblings[sibling_count - 1].digest = strdup(optarg);
            } break;

            case 'r': {
                ++sibling_count;
                siblings = realloc(siblings, sibling_count * sizeof(struct sibling));
                siblings[sibling_count - 1].dir = right;
                siblings[sibling_count - 1].digest = strdup(optarg);
            } break;

            default:
                verify_usage(argv[0]);
                break;
        }
    }

    if ( !rt || !msg || !siblings ) {
        fputs("A root, message, and at least one sibling must be specified.\n", stderr);
        goto cleanup;
    }

    if ( verify_merkle_path(rt, msg, siblings, sibling_count) ) {
        fprintf(stderr, "Proof verified!\n");
    } else {
        fprintf(stderr, "Invalid!\n");
    }

    cleanup:
        if ( siblings ) {
            for ( size_t i = 0; i < sibling_count; ++i ) {
                free(siblings[i].digest);
            }

            free(siblings);
        }
        if ( msg ) { free(msg); }
        if ( rt ) { free(rt); }
        return EXIT_SUCCESS;
}

_Noreturn void
verify_usage (const char * prog) {

    static const char header [] =
        "Usage: %s [options]\n"
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

