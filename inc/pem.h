#ifndef INCL_PEM_H
#define INCL_PEM_H

#pragma once

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <b64/cencode.h>
#include <b64/cdecode.h>

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

#endif
