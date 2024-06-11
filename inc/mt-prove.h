#ifndef INCL_MT_PROVE_H
#define INCL_MT_PROVE_H

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
    X("knowledge", "NUM", "K", "Prove knowledge of message at index NUM") \
    X("inclusion", "NUM", "I", "Prove inclusion of message at index NUM") \
    X("bytesize", "NUM", "b", "Override the tree's hash-length") \
    X("root-hex", "PATH", "r", "Write hex-encoded root to PATH; '-' treated as stdout") \
    X("root-pem", "PATH", "R", "Write pem-encoded root to PATH; '-' treated as stdout") \
    X("proof", "PATH", "p", "Write proof to PATH; '-' treated as stdout")

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
prove_usage (const char *);

#endif
