#ifndef INCL_CODEC_H
#define INCL_CODEC_H

#pragma once

#include <common.h>
#include <mtree.h>

struct textenc *
encode_mt (const struct mtree *);

struct mtree *
decode_mt (const struct textenc *, unsigned char **);

struct textenc *
encode_mr (const unsigned char *, size_t);

unsigned char *
decode_mr (const struct textenc *, size_t *);

#include <mproof.h>

struct textenc *
encode_mp (const struct mproof *);

struct mproof *
decode_mp (const struct textenc *);

#endif
