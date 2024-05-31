#include <msg.h>

signed
msg_from_str (
    size_t * msg_count,
    unsigned char *** messages,
    size_t ** msg_length,
    char * new_msg,
    size_t new_len
) {

    if ( !msg_count ) {
        return -1;
    }

    size_t old_cnt = *msg_count;
    if ( new_msg || new_len > 0 ) {
        size_t cnt = *msg_count;
        *msg_count = ++cnt;
        *messages = realloc(*messages, (sizeof *messages) * cnt);
        *msg_length = realloc(*msg_length, (sizeof *msg_length) * cnt);
        (*messages)[cnt - 1] = calloc(new_len + 1, (sizeof **messages));
        memcpy((*messages)[cnt - 1] + 1, new_msg, new_len);
        (*msg_length)[cnt - 1] = new_len + 1;
    }

    return !(*msg_count == old_cnt); // return 1 if message was ingested
}

signed
msg_from_file (
    size_t * msg_count,
    unsigned char *** messages,
    size_t ** msg_length,
    const char * path
) {

    if ( !msg_count ) {
        return -1;
    }

    size_t cnt = *msg_count;
    *msg_count = ++cnt;
    *messages = realloc(*messages, (sizeof *messages) * cnt);
    *msg_length = realloc(*msg_length, (sizeof *msg_length) * cnt);
    signed len = file2buf(path, &((*messages)[cnt - 1]));
    if ( len > 0 ) {
        (*msg_length)[cnt - 1] = len;
    }

    return 0;
}

signed
file2buf (const char * path, unsigned char ** buf) {

    if ( !buf ) {
        return -1;
    }

    if ( access(path, R_OK) != 0 ) {
        return -2;
    }

    FILE * f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    signed len = ftell(f);
    rewind(f);

    *buf = calloc(len + 1, (sizeof **buf));
    fread(*buf + 1, len, 1, f);
    fclose(f);

    return len + 1;
}

signed
random_msg (
    size_t * msg_count,
    unsigned char *** messages,
    size_t ** msg_length,
    size_t sz
) {
    if ( !msg_count || !sz ) {
        return -1;
    }

    char * buf = calloc(sz, sizeof(*buf));
    getrandom(buf, sz, 0);
    signed res = msg_from_str(msg_count, messages, msg_length, buf, sz);
    free(buf);
    return res;
}

