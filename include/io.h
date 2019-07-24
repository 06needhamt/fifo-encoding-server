#pragma once 

#include <stdio.h>
#include <stdlib.h>

#include <jansson.h>

#include <globals.h>
#include <guid.h>
#include <queue.h>
#include <queue_item.h>

extern void* on_error(const char* error);

long write_queue(FILE* file, char* file_name, queue_t* in);
long read_queue(FILE* file, char* file_name, queue_t* out);

long write_queues(FILE* file, char* file_name, unsigned count, queue_t* in[]);
long read_queues(FILE* file, char* file_name, unsigned count, queue_t* out[]);

long write_queue_item(FILE* file, char* file_name, queue_item_t* in);
long read_queue_item(FILE* file, char* file_name, queue_item_t* out);

long write_queue_items(FILE* file, char* file_name, unsigned count, queue_item_t* in[]);
long read_queue_items(FILE* file, char* file_name, unsigned count, queue_item_t* out[]);

