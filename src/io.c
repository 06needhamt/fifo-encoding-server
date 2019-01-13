#include <io.h>


int write_queue(FILE* file, queue_t* in) {
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);
    JSON_Value* items_array_value = json_value_init_array();
    JSON_Array* items_array = json_value_get_array(items_array_value);

    char* serialized_string = NULL;

    json_object_set_string(root_object, "guid", in->guid.value);
    json_object_set_number(root_object, "capacity", in->capacity);
    json_object_set_number(root_object, "item_count", in->item_count);
    for(long i = 0; i < in->item_count; i++){
        JSON_Value* item = item = json_value_init_object();
        JSON_Object* item_object = json_value_get_object(item);
        json_object_set_string(item_object, "guid", in->items[i].guid.value);
        json_object_set_string(item_object, "command", in->items[i].command);
        json_object_set_string(item_object, "source", in->items[i].source);
        json_object_set_string(item_object, "dest", in->items[i].dest);
        json_object_set_number(item_object, "item_type", in->items[i].item_type);
        json_object_set_number(item_object, "progress", in->items[i].progess);
        json_array_append_value(items_array, item);
    }
    json_object_set_value(root_object, "items", items_array_value);
    serialized_string = json_serialize_to_string_pretty(root_value);
    return fputs(serialized_string, file);
}

int read_queue(FILE* file, queue_t* out) {
    fseek(file, 0, SEEK_END); 
    long size = ftell(file);
    fseek(file, 0, SEEK_SET); 
    const char* json = (const char *) malloc(size);
    fread(json, 1, size, file);

    JSON_Value* root_value = json_parse_string_with_comments(json);
    JSON_Object* root_object = json_value_get_object(root_value);
    JSON_Value* items_array_value = json_object_get_array(root_object, "items");
    JSON_Array* items_array = json_value_get_array(items_array_value);

    create_guid(json_object_get_string(root_object, "guid"), &out->guid);
    out->capacity = json_object_get_number(root_object, "capacity");
    out->item_count = json_object_get_number(root_object, "item_count");
    for(long i = 0L; i < out->item_count; i++){
        JSON_Value* item_value = json_array_get_value(items_array, i);
        JSON_Object* item_object = json_value_get_object(item_value);
        create_guid(json_object_get_string(item_object, "guid"), &out->items[i].guid);
        out->items[i].command = json_object_get_string(item_object, "command");
        out->items[i].source = json_object_get_string(item_object, "source");
        out->items[i].dest = json_object_get_string(item_object, "dest");
        out->items[i].item_type = json_object_get_number(item_object, "item_type");
        out->items[i].progess = json_object_get_number(item_object, "progress");
    }
    return 1;
}

int write_queues(FILE* file, unsigned count, queue_t* in[]) {
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);
    JSON_Value* queues_array_value = json_value_init_array();
    JSON_Array* queues_array = json_value_get_array(queues_array_value);
    char* serialized_string = NULL;

    for(unsigned i = 0; i < count; i++) {
        JSON_Value* queue_value = json_value_init_object();
        JSON_Object* queue_object = json_value_get_object(queue_value);
        JSON_Value* items_array_value = json_value_init_array();
        JSON_Array* items_array = json_value_get_array(items_array_value);

        json_object_set_string(queue_object, "guid", in[i]->guid.value);
        json_object_set_number(queue_object, "capacity", in[i]->capacity);
        json_object_set_number(queue_object, "item_count", in[i]->item_count);
        for(long j = 0; j < in[i]->item_count; j++){
            JSON_Value* item = json_value_init_object();
            JSON_Object* item_object = json_value_get_object(item);
            json_object_set_string(item_object, "guid", in[i]->items[j].guid.value);
            json_object_set_string(item_object, "command", in[i]->items[j].command);
            json_object_set_string(item_object, "source", in[i]->items[j].source);
            json_object_set_string(item_object, "dest", in[i]->items[j].dest);
            json_object_set_number(item_object, "item_type", in[i]->items[j].item_type);
            json_object_set_number(item_object, "progress", in[i]->items[j].progess);
            json_array_append_value(items_array, item);
        }
        json_object_set_value(queue_object, "items", items_array_value);
        json_array_append_value(queues_array, queue_object);
    }
    json_object_set_value(root_object, "queues", queues_array_value);
    serialized_string = json_serialize_to_string_pretty(root_value);
    return fputs(serialized_string, file);
}

int read_queues(FILE* file, unsigned count, queue_t* out[]) {
    return fread(out, sizeof(queue_t), count, file);
}

int write_queue_item(FILE* file, queue_item_t* in) {
    JSON_Value* item = item = json_value_init_object();
    JSON_Object* item_object = json_value_get_object(item);
    char* serialized_string;
    
    json_object_set_string(item_object, "guid", in->guid.value);
    json_object_set_string(item_object, "command", in->command);
    json_object_set_string(item_object, "source", in->source);
    json_object_set_string(item_object, "dest", in->dest);
    json_object_set_number(item_object, "item_type", in->item_type);
    json_object_set_number(item_object, "progress", in->progess);
    serialized_string = json_serialize_to_string_pretty(item);
    return fputs(serialized_string, file);
}

int read_queue_item(FILE* file, queue_item_t* out) {
    return fread(out, sizeof(queue_item_t), 1, file);
}

int write_queue_items(FILE* file, unsigned count, queue_item_t* in[]) {
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);
    JSON_Value* items_array_value = json_value_init_array();
    JSON_Array* items_array = json_value_get_array(items_array_value);
    char* serialized_string;

    for(unsigned i = 0; i < count; i++) {
        JSON_Value* item = item = json_value_init_object();
        JSON_Object* item_object = json_value_get_object(item);
        
        json_object_set_string(item_object, "guid", in[i]->guid.value);
        json_object_set_string(item_object, "command", in[i]->command);
        json_object_set_string(item_object, "source", in[i]->source);
        json_object_set_string(item_object, "dest", in[i]->dest);
        json_object_set_number(item_object, "item_type", in[i]->item_type);
        json_object_set_number(item_object, "progress", in[i]->progess);
    }
    
    json_object_set_value(root_object, "items", items_array_value);
    serialized_string = json_serialize_to_string_pretty(root_value);
    return fputs(serialized_string, file);;
}

int read_queue_items(FILE* file, unsigned count, queue_item_t* out[]) {
    return fread(out, sizeof(queue_item_t), count, file);
}