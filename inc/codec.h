#ifndef INCL_CODEC_H
#define INCL_CODEC_H

#pragma once

#include <common.h>
#include <mtree.h>

struct textenc *
encode_mt (const struct mtree *);

struct mtree *
decode_mt (const struct textenc *, unsigned char **);

#endif
