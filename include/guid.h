#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct guid_t {
    char value[37];
} guid_t;

void create_new_guid(guid_t* out);

void create_guid(char guid[], guid_t* out);