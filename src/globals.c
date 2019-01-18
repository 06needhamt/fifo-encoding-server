#include <globals.h>

void* poll_thread() {
    while(1) {
        if(current_queue) {
            printf("Thread Polling for Work \n");
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