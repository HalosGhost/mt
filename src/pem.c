#include <pem.h>

struct textenc *
fr_txtenc (FILE * f) {

    base64_decodestate ctx;
    base64_init_decodestate(&ctx);

    struct textenc * res = calloc(1, sizeof (*res));
    size_t blk_len = 0;
    size_t blk_cap = 100;
    res->data = calloc(blk_cap, sizeof (*res->data));
    char opening_tag [1024] = "";
    char closing_tag [1024] = "";

    fscanf(f, "-----BEGIN %[^-]-----\n", opening_tag);
    char * line = NULL;
    size_t line_len = 0;
    while ( getline(&line, &line_len, f) != -1 ) {
        if ( sscanf(line, "-----END %[^-]-----\n", closing_tag) == 1 ) {
            break;
        }

        if ( blk_len + line_len > blk_cap ) {
            blk_cap *= 2;
            res->data = realloc(res->data, blk_cap * sizeof (*res->data));
        }

        blk_len += base64_decode_block(
            line, line_len, (char * )(res->data + blk_len), &ctx
        );
    }
    crypto_wipe(line, line_len);
    free(line);

    if ( !strcmp(opening_tag, closing_tag) ) {
        size_t tag_len = strlen(opening_tag) + 1;
        res->label = calloc(tag_len, sizeof(*res->label));
        memcpy(res->label, opening_tag, tag_len);
    } else {
        crypto_wipe(res->data, blk_cap * sizeof (*res->data));
        free(res->data);
        crypto_wipe(res, sizeof (*res));
        free(res);
        return NULL;
    }

    res->sz = blk_len;

    return res;
}

#define ceil_div(_n, _d) ((((_n) + (_d) - 1) / (_d)) + 1)
#define min(_l, _r) (((_l) < (_r)) ? (_l) : (_r))

signed
fw_txtenc (FILE * f, const struct textenc * txt) {

    if ( !f || !txt ) {
        return -1;
    }

    base64_encodestate ctx;
    base64_init_encodestate(&ctx);

    size_t len = 5 * ceil_div(txt->sz, 3);
    char * enc = calloc(len * 5, sizeof(*enc));
    char * cursor = enc;
    size_t out = base64_encode_block(
        (char * )txt->data,
        txt->sz,
        cursor,
        &ctx
    );
    cursor += out;
    out += base64_encode_blockend(cursor, &ctx);

    signed written = fprintf(f, "-----BEGIN %s-----\n", txt->label);
    signed o = 0; // counter for line-length
    for ( size_t i = 0; i < out; ++i ) {
        if ( enc[i] == '\n' ) {
            continue;
        }
        signed w = fwrite(enc + i, sizeof(*enc), 1, f);
        o += w;
        written += w;
        if ( o && (o % 63) == 0 && (i + 1) != out ) {
            written += fputc('\n', f);
        }
    }
    written += fprintf(f, "\n-----END %s-----\n", txt->label);
    fflush(f);

    return written;
}

void
free_txtenc (struct textenc * enc) {

    if ( enc ) {
        if ( enc->data ) { crypto_wipe(enc->data, enc->sz); free(enc->data); }
        if ( enc->label ) {
            crypto_wipe(enc->label, strlen(enc->label));
            free(enc->label);
        }
        crypto_wipe(enc, sizeof (*enc));
        free(enc);
    }
}

