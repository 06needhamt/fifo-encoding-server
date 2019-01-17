#pragma once

#include <pthread.h>
#include <unistd.h>

#include <guid.h>
#include <queue_item.h>

typedef struct thread {
    pthread_t handle;
    guid_t id;
    queue_item_t* item;
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

int create_thread_pool(int count, thread_pool_t* out);
int create_thread(queue_item_t* item, thread_t* out, thread_pool_t* pool);
int destroy_thread(thread_t* in, thread_pool_t* pool);
int destroy_thread_pool(thread_pool_t* in);
int park_thread(thread_t* in);
int unpark_thread(thread_t* in);
int abort_thread(thread_t* in);
int join_thread(thread_t* in);