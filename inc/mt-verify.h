#ifndef INCL_MT_VERIFY_H
#define INCL_MT_VERIFY_H

#pragma once

#include <stdio.h>
#include <getopt.h>

#include <common.h>
#include <hash.h>
#include <msg.h>
#include <mtree.h>

#define FOR_EACH_FLAG(X) \
    X("help", "h", "Print this help and exit")

#define DEF_FLG_OPTION_ST(_l, _s, _h) { _l, 0, 0, _s[0] },
#define FLG_OPTSTR(_l, _s, _h) _s

#define FOR_EACH_OPTION(X) \
    X("root", "HASH", "R", "The root to verify against") \
    X("left", "HASH", "l", "Add HASH as a sibling on the left") \
    X("right", "HASH", "r", "Add HASH as a sibling on the right") \
    X("from-str", "STR", "s", "Treat STR as the leaf to verify")

//    X("from-file", "PATH", "f", "Treat file at PATH as the leaf to verify")

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
verify_usage (const char *);

#endif
