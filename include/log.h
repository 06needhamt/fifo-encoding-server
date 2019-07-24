#pragma once

#include <stdio.h>

#include "globals.h"

#define LOG_LEVEL_CRITICAL "[CRITICAL]"
#define LOG_LEVEL_ERROR "[ERROR]"
#define LOG_LEVEL_WARNING "[WARNING]"
#define LOG_LEVEL_DEBUG "[DEBUG]"
#define LOG_LEVEL_INFO "[INFO]"
#define LOG_LEVEL_VERBOSE "[VERBOSE]"

#define LOG(fmt, log_level, ...) \
    fprintf(stderr, "%s:%s:%d:%s(): " log_level, fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__);