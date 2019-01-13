#include <queue_item.h>

int start_item(queue_item_t* item) {
    printf("Item Process Startng ...");
    printf("Command = %s", item->command);

    return 0;
}

int abort_item(queue_item_t* item) {
    return 0;
}

void create_item(char* command, char* source, char* dest, int item_type, queue_item_t* out) {
    create_new_guid(&out->guid);
    out->command = command;
    out->source = source;
    out->dest = dest;
    out->item_type = item_type;
    out->progess = 0;
}