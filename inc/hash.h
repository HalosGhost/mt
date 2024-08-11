#ifndef INCL_HASH_H
#define INCL_HASH_H

#pragma once

#include <common.h>

signed
get_hash (const unsigned char *, size_t, unsigned char *, size_t);

signed
const_cmp (const unsigned char *, size_t, const unsigned char *, size_t);

#endif
