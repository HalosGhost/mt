#include <mt-create.h>

signed
main (signed argc, char * argv []) {

    size_t hash_len = 64;
    unsigned char ** messages = NULL;
    size_t * msg_length = NULL;
    size_t msg_count = 0;
    size_t leaf_count = 0;
    unsigned char * rt = NULL;

    signed oi = 0, c = 0;
    while ( (c = getopt_long(argc, argv, optstr, os, &oi)) != -1 ) {
        switch ( c ) {
            case 'b':
                if ( optarg ) { sscanf(optarg, "%zu", &hash_len); }
                if ( hash_len > 64 ) {
                    fprintf(stderr, "%zu is too large; defaulting to 64\n", hash_len);
                    hash_len = 64;
                } break;

            case 'd':
                random_msg(&msg_count, &messages, &msg_length, hash_len);
                break;

            case 's':
                if ( optarg ) {
                    size_t len = strlen(optarg);
                    msg_from_str(
                        &msg_count,
                        &messages,
                        &msg_length,
                        optarg,
                        len
                    );
                } break;

            case 'f':
                if ( optarg ) {
                    msg_from_file(
                        &msg_count,
                        &messages,
                        &msg_length,
                        optarg
                    );
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

    { // guarantee power-of-two leaves for trivially balanced binary tree
        if ( !leaf_count && (!is_pow2(msg_count) || msg_count <= 1) ) {
            leaf_count = next_pow2(msg_count + 1);
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

            for ( size_t i = msg_count; i < leaf_count; ++i ) {
                random_msg(&msg_count, &messages, &msg_length, hash_len);
            }
        }
    }

    rt = get_merkle_root(msg_count, messages, msg_length, hash_len);
    printf("root: %s\n", rt);

    cleanup:
        if ( messages ) {
            for ( size_t i = 0; i < msg_count; ++i ) {
                free(messages[i]);
            }
        }
        if ( rt ) { free(rt); }
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

