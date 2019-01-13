#pragma once

#include <stdlib.h>
#include <stdio.h>

#include <guid.h>
#include <queue_item.h>

typedef struct queue {
    guid_t guid;
    long item_count;
    long capacity;
    queue_item_t* items;
} queue_t;

void push_item(queue_t* q, queue_item_t* item);
void pop_item(queue_t* q, queue_item_t* out);
void create_queue(queue_t* out, long capacity);
