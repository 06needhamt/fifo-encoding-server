#include <stdlib.h>
#include <stdio.h>

#include <guid.h>
#include <queue.h>
#include <queue_item.h>

void push_item(queue_t* q, queue_item_t* item){
    if(q->item_count >= q->capacity){
        printf("[ERROR] Queue Full \n");
        return;
    };

    q->items[q->item_count++] = *item;
}

queue_item_t pop_item(queue_t* q){
    if(q->item_count == 0) {
        printf("[ERROR] Queue Empty \n");;
    }

    return q->items[--q->item_count];
}

void create_queue(queue_t* out, long capacity) {
    create_new_guid(&out->guid);
    out->capacity = capacity;
    out->item_count = 0L;
    out->items = (queue_item_t*) malloc(capacity * sizeof(queue_item_t));
}