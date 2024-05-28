#ifndef INCL_MTREE_H
#define INCL_MTREE_H

#pragma once

#include <math.h>

#include <common.h>
#include <hash.h>

unsigned char *
get_merkle_root (size_t, unsigned char *[], size_t [], size_t);

#endif
