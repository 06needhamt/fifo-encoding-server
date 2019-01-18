#include <globals.h>

void* poll_thread() {
    while(1) {
        if(current_queue) {
            if(current_queue->item_count > 0){
                current_item = pop_item(current_queue);
                start_item(&current_item);
            }
        }
        nanosleep(0, (15000 * (int) pow(10, 6)));
    }
    return NULL;
}