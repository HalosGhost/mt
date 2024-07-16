#ifndef INCL_MPROOF_H
#define INCL_MPROOF_H

#pragma once

#include <common.h>
#include <mtree.h>

enum proof_type {
    of_inclusion,
    of_knowledge
};

struct mproof {
    unsigned char * msg;
    unsigned char ** elements;
    size_t element_count;
    size_t msg_sz;
    size_t hash_sz;
    enum proof_type t;
};

struct mproof *
proof_from_tree (const struct mtree *, size_t, size_t, enum proof_type);

bool
is_valid_proof (struct mproof *);

void
free_proof (struct mproof *);

#endif
