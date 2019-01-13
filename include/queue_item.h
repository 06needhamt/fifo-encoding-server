#pragma once

#include <stdlib.h>
#include <stdio.h>

#include <guid.h>

typedef struct queue_item {
    guid_t guid;
    char* source;
    char* dest;
    int item_type;
    int progess; 
    char* command;
} queue_item_t;

int start_item(queue_item_t* item);
int abort_item(queue_item_t* item);
void create_item(char* command, char* source, char* dest, int item_type, queue_item_t* out);