#pragma once 

#include<stdio.h>
#include<stdlib.h>
#include <parson.h>

#include <guid.h>
#include <queue.h>
#include <queue_item.h>

int write_queue(FILE* file, queue_t* in);
int read_queue(FILE* file, queue_t* out);

int write_queues(FILE* file, unsigned count, queue_t* in[]);
int read_queues(FILE* file, unsigned count, queue_t* out[]);

int write_queue_item(FILE* file, queue_item_t* in);
int read_queue_item(FILE* file, queue_item_t* out);

int write_queue_items(FILE* file, unsigned count, queue_item_t* in[]);
int read_queue_items(FILE* file, unsigned count, queue_item_t* out[]);
