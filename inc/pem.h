#ifndef INCL_PEM_H
#define INCL_PEM_H

#pragma once

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <arpa/inet.h>

#include <b64/cencode.h>
#include <b64/cdecode.h>

#include <monocypher.h>

// parbaked RFC7468 structure and codec

struct textenc {
    char * label;
    unsigned char * data;
    size_t sz;
};

struct textenc *
fr_txtenc (FILE *);

signed
fw_txtenc (FILE *, const struct textenc *);

void
free_txtenc (struct textenc *);

// cf. https://stackoverflow.com/a/47380224
#define htonll(_x) \
    ((1 == htonl(1)) ? (_x) \
                     : ((((uint64_t )htonl((_x) & 0xFFFFFFFFUL)) << 32) \
                     | htonl((uint32_t )((_x) >> 32))))
#define ntohll(_x) \
    ((1 == ntohl(1)) ? (_x) \
                     : ((((uint64_t )ntohl((_x) & 0xFFFFFFFFUL)) << 32) \
                     | ntohl((uint32_t )((_x) >> 32))))

#endif
