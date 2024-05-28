#ifndef INCL_MSG_H
#define INCL_MSG_H

#pragma once

#include <stdio.h>
#include <sys/random.h>

#include <common.h>

signed
msg_from_str (size_t *, unsigned char ***, size_t **, char *, size_t);

signed
random_msg (size_t *, unsigned char ***, size_t **, size_t);

#endif
