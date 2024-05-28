#ifndef INCL_MT_CREATE_H
#define INCL_MT_CREATE_H

#pragma once

#include <stdio.h>
#include <getopt.h>

#include <common.h>
#include <hash.h>
#include <msg.h>
#include <mtree.h>

#define FOR_EACH_FLAG(X) \
    X("help", "h", "Print this help and exit") \
    X("decoy", "d", "Add randomly-rolled leaf as a decoy")

#define DEF_FLG_OPTION_ST(_l, _s, _h) { _l, 0, 0, _s[0] },
#define FLG_OPTSTR(_l, _s, _h) _s

#define FOR_EACH_OPTION(X) \
    X("bytesize", "NUM", "b", "Truncate hashes to NUM bytes (default: 64)") \
    X("from-str", "STR", "s", "Include string STR as a leaf") \
    X("leaf-count", "NUM", "l", "Include NUM leaves (randomly rolling more if-needed)")

//    X("from-file", "PATH", "f", "Include file at PATH as a leaf")

#define DEF_OPT_OPTION_ST(_l, _a, _s, _h) { _l, 1, 0, _s[0] },
#define OPT_OPTSTR(_l, _a, _s, _h) _s ":"

static struct option os [] = {
    FOR_EACH_FLAG(DEF_FLG_OPTION_ST)
    FOR_EACH_OPTION(DEF_OPT_OPTION_ST)
    { 0, 0, 0, 0 }
};

static const char optstr [] =
    FOR_EACH_FLAG(FLG_OPTSTR)
    FOR_EACH_OPTION(OPT_OPTSTR)
;

void
create_usage (const char *);

// cf. https://graphics.stanford.edu/%7Eseander/bithacks.html#DetermineIfPowerOf2
#define is_pow2(_v) (((_v) & ((_v) - 1)) == 0)

// cf. https://graphics.stanford.edu/%7Eseander/bithacks.html#RoundUpPowerOf2
size_t
next_pow2 (size_t);

#endif
