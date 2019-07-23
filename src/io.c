#include <io.h>


long write_queue(FILE* file, char* file_name, queue_t* in) {
    long err;
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
        err = json_object_set(item_object, "input_file_name", json_string(in->items[i].input_file_name));
        err = json_object_set(item_object, "output_file_name", json_string(in->items[i].output_file_name));
        err = json_object_set(item_object, "item_type", json_integer(in->items[i].item_type));
        err = json_object_set(item_object, "progress", json_integer(in->items[i].progress));

        err = json_array_append(items_array, item_object);
    }
    err = json_object_set(root, "items", items_array);
	
	fclose(file);
	file = fopen(file_name, "w");
    err = json_dumpf(root, file, JSON_INDENT(4));
	fflush(file);
    return err;
}

long read_queue(FILE* file, char* file_name, queue_t* out) {
    json_error_t* err = malloc(sizeof(json_error_t)); 
    fseek(file, 0, SEEK_END); 
    long json_size = ftell(file); 
    fseek(file, 0, SEEK_SET); 
    const char* json = (const char*) malloc(sizeof(char) * (json_size + 1));
    fread(json, sizeof(char), json_size + 1, file);
    json_t* root = json_loads(json, 0, err);
    
    create_guid(json_string_value(json_object_get(root, "guid")), &out->guid);
    out->item_count = (long) json_number_value(json_object_get(root, "item_count"));
    out->capacity = (long) json_number_value(json_object_get(root, "capacity"));
    out->items = malloc(out->item_count * sizeof(queue_item_t));

    json_t* items_array = json_object_get(root, "items");

    for(long i = 0; i < json_array_size(items_array); i++){
       json_t* item_object = json_array_get(items_array, i);
       queue_item_t* item = malloc(sizeof(queue_item_t));
       create_guid(json_string_value(json_object_get(item_object, "guid")), &item->guid);
       item->command = json_string_value(json_object_get(item_object, "command"));
       item->source = json_string_value(json_object_get(item_object, "source"));
       item->dest = json_string_value(json_object_get(item_object, "dest"));
       item->input_file_name = json_string_value(json_object_get(item_object, "input_file_name"));
       item->output_file_name = json_string_value(json_object_get(item_object, "output_file_name"));
       item->item_type = (int) json_number_value(json_object_get(item_object, "item_type"));
       item->progress = (int) json_number_value(json_object_get(item_object, "progress"));
       out->items[i] = *item;
    }
    return (long)err;
}

long write_queues(FILE* file, char* file_name, unsigned count, queue_t* in[]) {
    long err;
    json_t* root = json_object();
	json_t* queues_array = json_array();

	for(unsigned i = 0; i < count; i++) {
		json_t* queue_object = json_object();
		json_t* items_array = json_array();

		err = json_object_set(queue_object, "guid", json_string(in[i]->guid.value));
		err = json_object_set(queue_object, "item_count", json_integer(in[i]->item_count));
		err = json_object_set(queue_object, "capacity", json_integer(in[i]->capacity));

		for(long j = 0; j < in[i]->item_count; i++){
			json_t* item_object = json_object();

			err = json_object_set(item_object, "guid", json_string(in[i]->items[j].guid.value));
			err = json_object_set(item_object, "command", json_string(in[i]->items[j].command));
			err = json_object_set(item_object, "source", json_string(in[i]->items[j].source));
			err = json_object_set(item_object, "dest", json_string(in[i]->items[j].dest));
			err = json_object_set(item_object, "input_file_name", json_string(in[i]->items[j].input_file_name));
			err = json_object_set(item_object, "output_file_name", json_string(in[i]->items[j].output_file_name));
			err = json_object_set(item_object, "item_type", json_integer(in[i]->items[j].item_type));
			err = json_object_set(item_object, "progress", json_integer(in[i]->items[j].progress));

			err = json_array_append(items_array, item_object);
		}
		err = json_object_set(queue_object, "items", items_array);
		err = json_array_append(queues_array, queue_object);
	}
	err = json_object_set(root, "queues", queues_array);
	
	fclose(file);
	file = fopen(file_name, "w");
    err = json_dumpf(root, file, JSON_INDENT(4));
	fflush(file);

    return err;
}

long read_queues(FILE* file, char* file_name, unsigned count, queue_t* out[]) {
    json_error_t* err = malloc(sizeof(json_error_t)); 
    fseek(file, 0, SEEK_END); 
    long json_size = ftell(file); 
    fseek(file, 0, SEEK_SET); 
    const char* json = (const char*) malloc(sizeof(char) * (json_size + 1));
    fread(json, sizeof(char), json_size + 1, file);

    json_t* root = json_loads(json, 0, err);
	json_t* queues_array = json_object_get(root, "queues");

	for(long i = 0; i < json_array_size(queues_array); i++) {
		json_t* queue_object = json_array_get(queues_array, i);
		queue_t* queue = malloc(sizeof(queue_t));

		create_guid(json_string_value(json_object_get(queue_object, "guid")), &queue->guid);
		queue->item_count = (long) json_number_value(json_object_get(queue_object, "item_count"));
		queue->capacity = (long) json_number_value(json_object_get(queue_object, "capacity"));
		queue->items = malloc(queue->item_count * sizeof(queue_item_t));

		json_t* items_array = json_object_get(queue_object, "items");

		for(long j = 0; j < json_array_size(items_array); i++){
			json_t* item_object = json_array_get(items_array, j);
			queue_item_t* item = malloc(sizeof(queue_item_t));

			create_guid(json_string_value(json_object_get(item_object, "guid")), &item->guid);
			item->command = json_string_value(json_object_get(item_object, "command"));
			item->source = json_string_value(json_object_get(item_object, "source"));
			item->dest = json_string_value(json_object_get(item_object, "dest"));
			item->input_file_name = json_string_value(json_object_get(item_object, "input_file_name"));
			item->output_file_name = json_string_value(json_object_get(item_object, "output_file_name"));
			item->item_type = (int) json_number_value(json_object_get(item_object, "item_type"));
			item->progress = (int) json_number_value(json_object_get(item_object, "progress"));
			queue->items[j] = *item;
		}
		out[i] = queue;
	}
	return (long)err;
}

long write_queue_item(FILE* file, char* file_name, queue_item_t* in) {
    long err;
    json_t* root = json_object();

	err = json_object_set(root, "guid", json_string(in->guid.value));
    err = json_object_set(root, "command", json_string(in->command));
    err = json_object_set(root, "source", json_string(in->source));
    err = json_object_set(root, "dest", json_string(in->dest));
    err = json_object_set(root, "input_file_name", json_string(in->input_file_name));
    err = json_object_set(root, "output_file_name", json_string(in->output_file_name));
    err = json_object_set(root, "item_type", json_integer(in->item_type));
    err = json_object_set(root, "progress", json_integer(in->progress));

	fclose(file);
	file = fopen(file_name, "w");
    err = json_dumpf(root, file, JSON_INDENT(4));
	fflush(file);
	
	return err;
}

long read_queue_item(FILE* file, char* file_name, queue_item_t* out) {
	json_error_t* err = malloc(sizeof(json_error_t)); 
    fseek(file, 0, SEEK_END); 
    long json_size = ftell(file); 
    fseek(file, 0, SEEK_SET); 
    const char* json = (const char*) malloc(sizeof(char) * (json_size + 1));
    fread(json, sizeof(char), json_size + 1, file);
    json_t* root = json_loads(json, 0, err);

    create_guid(json_string_value(json_object_get(root, "guid")), &out->guid);
    out->command = json_string_value(json_object_get(root, "command"));
    out->source = json_string_value(json_object_get(root, "source"));
    out->dest = json_string_value(json_object_get(root, "dest"));
    out->input_file_name = json_string_value(json_object_get(root, "input_file_name"));
    out->output_file_name = json_string_value(json_object_get(root, "output_file_name"));
    out->item_type = (int) json_number_value(json_object_get(root, "item_type"));
    out->progress = (int) json_number_value(json_object_get(root, "progress"));
    return (long)err;
}

long write_queue_items(FILE* file, char* file_name, unsigned count, queue_item_t* in[]) {
    long err;
    json_t* root = json_object();
	json_t* items_array = json_array();

	for(unsigned i = 0; i < count; i++) {
		json_t* item_object = json_object();
		err = json_object_set(item_object, "guid", json_string(in[i]->guid.value));
		err = json_object_set(item_object, "command", json_string(in[i]->command));
		err = json_object_set(item_object, "source", json_string(in[i]->source));
		err = json_object_set(item_object, "dest", json_string(in[i]->dest));
		err = json_object_set(item_object, "input_file_name", json_string(in[i]->input_file_name));
		err = json_object_set(item_object, "output_file_name", json_string(in[i]->output_file_name));
		err = json_object_set(item_object, "item_type", json_integer(in[i]->item_type));
		err = json_object_set(item_object, "progress", json_integer(in[i]->progress));

		err = json_array_append(items_array, item_object);
	}
	err = json_object_set(root, "items", items_array);
	
	fclose(file);
	file = fopen(file_name, "w");
    err = json_dumpf(root, file, JSON_INDENT(4));
	fflush(file);
	
	return err;
}

long read_queue_items(FILE* file, char* file_name, unsigned count, queue_item_t* out[]) {
   	json_error_t* err = malloc(sizeof(json_error_t)); 
    fseek(file, 0, SEEK_END); 
    long json_size = ftell(file); 
    fseek(file, 0, SEEK_SET); 
    const char* json = (const char*) malloc(sizeof(char) * (json_size + 1));
    fread(json, sizeof(char), json_size + 1, file);

    json_t* root = json_loads(json, 0, err);
    json_t* items_array = json_object_get(root, "items");

    for(unsigned i = 0; i < json_array_size(items_array); i++) {
        json_t* item_object = json_array_get(items_array, i);
        create_guid(json_string_value(json_object_get(item_object, "guid")), &out[i]->guid);
        out[i]->command = json_string_value(json_object_get(item_object, "command"));
        out[i]->source = json_string_value(json_object_get(item_object, "source"));
        out[i]->dest = json_string_value(json_object_get(item_object, "dest"));
        out[i]->input_file_name = json_string_value(json_object_get(item_object, "input_file_name"));
        out[i]->output_file_name = json_string_value(json_object_get(item_object, "output_file_name"));
        out[i]->item_type = (int) json_number_value(json_object_get(item_object, "item_type"));
        out[i]->progress = (int) json_number_value(json_object_get(item_object, "progress"));
    }
    return (long)err;
}