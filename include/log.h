#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <globals.h>

#define LOG_PREFIX __FILE__ __FUNCTION__ __LINE__

void log_both(const char* message, ...);

void log_verbose(const char* message, ...);
void log_info(const char* message, ...);
void log_debug(const char* message, ...);
void log_warning(const char* message, ...);
void log_error(const char* message, ...);
void log_fatal(const char* message, ...);
