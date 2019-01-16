#include <queue_item.h>

int start_item(queue_item_t* item) {
    printf("Item Process Startng ...");
    if(item->command == "Transcode" && item ->item_type == 1);
        return transcode_video(item->input_file_name, item->output_file_name);

    return -1;
}

int abort_item(queue_item_t* item) {
    return 0;
}

void create_item(char* command, char* source, char* dest, char* input_file_name, char* output_file_name, int item_type, queue_item_t* out) {
    create_new_guid(&out->guid);
    out->command = command;
    out->source = source;
    out->dest = dest;
    out->input_file_name = input_file_name;
    out->output_file_name = output_file_name;
    out->item_type = item_type;
    out->progress = 0;
}