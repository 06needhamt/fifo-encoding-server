#include <io.h>


int write_queue(FILE* file, queue_t* in) {
    return 1;
}

int read_queue(FILE* file, queue_t* out) {
    return 1;
}

int write_queues(FILE* file, unsigned count, queue_t* in[]) {
    return 1;
}

int read_queues(FILE* file, unsigned count, queue_t* out[]) {
    return fread(out, sizeof(queue_t), count, file);
}

int write_queue_item(FILE* file, queue_item_t* in) {
    return 1;
}

int read_queue_item(FILE* file, queue_item_t* out) {
    return fread(out, sizeof(queue_item_t), 1, file);
}

int write_queue_items(FILE* file, unsigned count, queue_item_t* in[]) {
    return 1;
}

int read_queue_items(FILE* file, unsigned count, queue_item_t* out[]) {
    return fread(out, sizeof(queue_item_t), count, file);
}