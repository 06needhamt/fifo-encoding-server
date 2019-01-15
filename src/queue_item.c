#include <queue_item.h>

int start_item(queue_item_t* item) {
    printf("Item Process Startng ...");
    printf("Command = %s", item->command);

    return 0;
}

int abort_item(queue_item_t* item) {
    return 0;
}

void create_item(char* command, char* source, char* dest, char* file_name, int item_type, queue_item_t* out) {
    create_new_guid(&out->guid);
    out->command = command;
    out->source = source;
    out->dest = dest;
    out->file_name = file_name;
    out->item_type = item_type;
    out->progress = 0;
}