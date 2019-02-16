#pragma once 

#include <stdio.h>
#include <stdlib.h>

#include <jansson.h>

#include <globals.h>
#include <guid.h>
#include <queue.h>
#include <queue_item.h>

extern void* on_error(const char* error);

int write_queue(FILE* file, char* file_name, queue_t* in);
int read_queue(FILE* file, char* file_name, queue_t* out);

int write_queues(FILE* file, char* file_name, unsigned count, queue_t* in[]);
int read_queues(FILE* file, char* file_name, unsigned count, queue_t* out[]);

int write_queue_item(FILE* file, char* file_name, queue_item_t* in);
int read_queue_item(FILE* file, char* file_name, queue_item_t* out);

int write_queue_items(FILE* file, char* file_name, unsigned count, queue_item_t* in[]);
int read_queue_items(FILE* file, char* file_name, unsigned count, queue_item_t* out[]);

