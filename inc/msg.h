#ifndef INCL_MSG_H
#define INCL_MSG_H

#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/random.h>

#include <common.h>

signed
msg_from_str (size_t *, unsigned char ***, size_t **, char *, size_t);

signed
msg_from_file (size_t *, unsigned char ***, size_t **, const char *);

signed
file2buf (const char *, unsigned char **);

signed
random_msg (size_t *, unsigned char ***, size_t **, size_t);

#endif
