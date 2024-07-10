#ifndef INCL_MTREE_H
#define INCL_MTREE_H

#pragma once

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#include <common.h>
#include <hash.h>
#include <msg.h>

#define FOR_EACH_MSGTYPE(X) \
    X(from_file) \
    X(from_str) \
    X(decoy)

#define DECL_MSGTYPE_ENUM(_n) _n,
enum msg_type {
    FOR_EACH_MSGTYPE(DECL_MSGTYPE_ENUM)
};

#define DECL_MSGTYPE_NM(_n) [_n] = #_n,
static const char * __attribute__((used)) msg_type_name [] = {
    FOR_EACH_MSGTYPE(DECL_MSGTYPE_NM)
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
