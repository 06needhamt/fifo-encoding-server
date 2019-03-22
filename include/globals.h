#pragma once
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"

#define QUEUE_CAPACITY 100

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/sysinfo.h>

#include <guid.h>
#include <queue.h>
#include <queue_item.h>
#include <threads.h>
#include <io.h>
#include <transcode.h>

extern queue_t* current_queue;
extern queue_item_t current_item; 
extern thread_pool_t* current_pool;

extern FILE* log_file;
extern FILE* data_file;

char* data_file_path;
char* log_file_path;

int MAX_THREADS;

void* poll_thread(thread_data_t** data);

