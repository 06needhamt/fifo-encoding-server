#pragma once

#define QUEUE_CAPACITY 100

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/sysinfo.h>

#include <guid.h>
#include <queue.h>
#include <queue_item.h>
#include <threads.h>
#include <io.h>

static thread_pool_t* pool = NULL;
static queue_t* current_queue = NULL;

static queue_item_t current_item = {0};

static FILE* log_file = NULL;
static FILE* data_file = NULL;

static int MAX_THREADS;

void* poll_thread();

