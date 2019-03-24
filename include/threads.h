#pragma once

#include <pthread.h>
#include <unistd.h>

#include <guid.h>
#include <queue_item.h>
#include <transcode.h>

typedef struct thread_data {
	int tid;
	TranscodingContext* tra_ctx;
	queue_item_t* item;
} thread_data_t;

typedef struct thread {
    pthread_t handle;
    guid_t id;
	thread_data_t* data;
    int busy;
} thread_t;

typedef struct thread_pool {
    thread_t* pool;
    guid_t id;
    int thread_count;
    int idle_count;
    int busy_count;
} thread_pool_t;

extern char busy;
extern int park();
extern int unpark();
extern void* poll_thread();

int create_thread_pool(int count, thread_pool_t* out);
int create_thread_data(int tid, thread_data_t* out);
int create_thread(queue_item_t* item, thread_t* out, thread_pool_t* pool);
int destroy_thread(thread_t* in, thread_pool_t* pool);
int destroy_thread_pool(thread_pool_t* in);
int park_thread(thread_t* in);
int unpark_thread(thread_t* in);
int abort_thread(thread_t* in);
int join_thread(thread_t* in);
