#ifndef INCL_MT_VERIFY_H
#define INCL_MT_VERIFY_H

#pragma once

#include <stdio.h>
#include <getopt.h>

#include <common.h>
#include <hash.h>
#include <msg.h>
#include <mtree.h>
#include <mproof.h>
#include <codec.h>

#define FOR_EACH_FLAG(X) \
    X("help", "h", "Print this help and exit")

#define DEF_FLG_OPTION_ST(_l, _s, _h) { _l, 0, 0, _s[0] },
#define FLG_OPTSTR(_l, _s, _h) _s

#define FOR_EACH_OPTION(X) \
    X("root-hex", "STR", "r", "Verify against the hex-encoded root in STR") \
    X("root-pem", "PATH", "R", "Verify against The textual encoded root at PATH") \
    X("out", "PATH", "o", "Write verified file to PATH; '-' treated as stdout)")

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
