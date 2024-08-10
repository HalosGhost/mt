#ifndef INCL_MSG_H
#define INCL_MSG_H

#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/random.h>

#include <common.h>

signed
file_to_buf (const char *, unsigned char **);

#endif
