#include <stdlib.h>
#include <stdio.h>

#include <guid.h>
#include <queue.h>
#include <queue_item.h>

void push_item(queue_t* q, queue_item_t* item){
    if(q->item_count >= MAX_ITEMS){
        printf("[ERROR] Queue Full \n");
        return;
    };

    q->items[q->item_count++] = item;
}

queue_item_t* pop_item(queue_t* q){
    if(q->item_count == 0) {
        printf("[ERROR] Queue Empty \n");
        return NULL;
    }

    return q->items[--q->item_count];
}