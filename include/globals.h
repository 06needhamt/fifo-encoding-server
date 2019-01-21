#pragma once

#define QUEUE_CAPACITY 100

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/sysinfo.h>

#include <guid.h>
#include <queue.h>
#include <queue_item.h>
#include <threads.h>
#include <io.h>

extern queue_t* current_queue;
extern queue_item_t current_item; 

int MAX_THREADS;

void* poll_thread(int tid);

