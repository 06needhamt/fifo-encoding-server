#include <globals.h>

void* poll_thread(int tid) {
    while(1) {
        if(current_queue) {
            printf("Thread %i Polling for Work \n", tid);
            if(current_queue->item_count > 0){
                current_item = pop_item(current_queue);
                start_item(&current_item);
            }
            park();
        }
        sleep(5);
    }
    return NULL;
}