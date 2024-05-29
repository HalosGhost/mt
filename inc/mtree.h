#ifndef INCL_MTREE_H
#define INCL_MTREE_H

#pragma once

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#include <common.h>
#include <hash.h>

enum direction {
    left, right
};

struct sibling {
    unsigned char * digest;
    enum direction dir;
};

unsigned char *
get_merkle_root (size_t, unsigned char *[], size_t [], size_t);

bool
verify_merkle_path (
    const unsigned char *,
    const unsigned char *,
    const struct sibling *,
    size_t
);

#endif
