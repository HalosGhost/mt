#ifndef INCL_MT_STAT_H
#define INCL_MT_STAT_H

#pragma once

#include <stdio.h>
#include <getopt.h>

#include <common.h>
#include <hash.h>
#include <msg.h>
#include <mtree.h>
#include <codec.h>

#define FOR_EACH_FLAG(X) \
    X("help", "h", "Print this help and exit")

#define DEF_FLG_OPTION_ST(_l, _s, _h) { _l, 0, 0, _s[0] },
#define FLG_OPTSTR(_l, _s, _h) _s

#define FOR_EACH_OUTFMT(X) \
    X(0, none) \
    X(1, list) \
    X(2, tree)

#define FMT(_n) fmt_ ## _n

#define DECL_ENUM(_id, _n) FMT(_n) = _id,
enum out_fmt {
    FOR_EACH_OUTFMT(DECL_ENUM)
};

#define MATCH_FMT(_s, _n) (!strncmp((_s), FMTNAME(_n), strlen(FMTNAME(_n))))
#define HANDLE_FMT_MATCH(_id, _n) if ( MATCH_FMT(optarg, _n) ) { fmt = FMT(_n); } else

#define FMTNAME(_n) (fmt_name[FMT(_n)])

#define DECL_FMTSTR(_id, _n) [_id] = #_n,
static const char * fmt_name [] = {
    FOR_EACH_OUTFMT(DECL_FMTSTR)
};

#define OUTFMT_HELP_OPTION(_id, _n) "  '" #_n "'"
static const char * fmt_opts = "One of:"
    FOR_EACH_OUTFMT(OUTFMT_HELP_OPTION)
    "."
;

#define FOR_EACH_OPTION(X) \
    X("format", "FMT", "f", fmt_opts)

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
stat_usage (const char *);

void
list_leaves (const struct mtree *);

void
draw_tree (const struct mtree *);

#endif
