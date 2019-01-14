#include <io.h>


int write_queue(FILE* file, queue_t* in) {
    int err;
    json_t* root = json_object();
    json_t* items_array = json_array();

    err = json_object_set(root, "guid", json_string(in->guid.value));
    err = json_object_set(root, "item_count", json_integer(in->item_count));
    err = json_object_set(root, "capacity", json_integer(in->capacity));

    for(long i = 0; i < in->item_count; i++){
        json_t* item_object = json_object();

        err = json_object_set(item_object, "guid", json_string(in->items[i].guid.value));
        err = json_object_set(item_object, "command", json_string(in->items[i].command));
        err = json_object_set(item_object, "source", json_string(in->items[i].source));
        err = json_object_set(item_object, "dest", json_string(in->items[i].dest));
        err = json_object_set(item_object, "item_type", json_integer(in->items[i].item_type));
        err = json_object_set(item_object, "progress", json_integer(in->items[i].progress));

        err = json_array_append(items_array, item_object);
    }
    err = json_object_set(root, "items", items_array);
    err = json_dumpf(root, file, 0);
    return err;
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