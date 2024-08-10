#include <msg.h>

signed
file_to_buf (const char * path, unsigned char ** buf) {

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

    *buf = calloc(len, (sizeof **buf));
    fread(*buf, len, 1, f);
    fclose(f);

    return len;
}

