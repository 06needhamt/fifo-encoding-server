#pragma once

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

#include <globals.h>

#define LOG_PREFIX __FILE__ __FUNCTION__ __LINE__
#define MAX_MATCHES 10

static const char REGEX[7] = "{[0-9]}";

void log_both(const char* message, ...);

void log_verbose(const char* message, ...);
void log_info(const char* message, ...);
void log_debug(const char* message, ...);
void log_warning(const char* message, ...);
void log_error(const char* message, ...);
void log_fatal(const char* message, ...);

int match_format_specifier(const char* message);
int match_count(regex_t *pexp, char *sz);
