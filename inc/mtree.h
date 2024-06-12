#ifndef INCL_MTREE_H
#define INCL_MTREE_H

#pragma once

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#include <common.h>
#include <hash.h>
#include <msg.h>

enum msg_type {
    from_file,
    from_str,
    decoy
};

struct mt_msg {
    unsigned char * data;
    char * location;
    size_t sz;
    enum msg_type t;
};

struct mtree {
    struct mt_msg * leaves;
    size_t leaf_count;
    size_t hash_sz;
};

struct mtree *
create_mt (size_t, enum msg_type [], char *[], size_t);

void
free_mt (struct mtree *);

struct textenc *
encode_mt (const struct mtree *);

unsigned char *
root_from_tree (const struct mtree *, size_t);

unsigned char *
get_merkle_root (size_t, unsigned char *[], size_t [], size_t);

bool
verify_merkle_path (
    const unsigned char *,
    const unsigned char *,
    size_t,
    const unsigned char *[],
    size_t
);

#endif
