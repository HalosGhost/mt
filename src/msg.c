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
        (*msg_length)[cnt - 1] = new_len;
        (*messages)[cnt - 1] = calloc(new_len, (sizeof **messages));
        memcpy((*messages)[cnt - 1], new_msg, new_len);
    }

    return !(*msg_count == old_cnt); // return 1 if message was ingested
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

