#include <globals.h>

void* poll_thread(thread_data_t* data) {
    while(true) {
        printf("Thread %i Polling for Work \n", data->tid);
        if(current_queue && !isPolling) {
			isPolling = true;
            if(current_queue->item_count > 0 && data->item == NULL) {
				data->item = (queue_item_t*) malloc(sizeof(queue_item_t));
    			write_queue(data_file, data_file_path, current_queue);
                *(data->item) = pop_item(current_queue);
				isPolling = false;

				if(data->item) {
					start_item(data->item, data->tra_ctx);
					free(data->item);
					data->item = NULL;
				}
            }
            park();
        }
		isPolling = false;
        sleep(5);
    }
    return NULL;
}