#include <http_request.h>

int create_http_request_header(const char* header, const char* value, http_request_header_t*  out) {
    out->header = strdup(header);
    out->value = strdup(value);
    return 1;
}

int create_http_request(const http_request_header_t* request_headers, const char* url, http_request_type_t request_type,
			size_t header_count, const char* request_body, size_t body_length, const char* origin, http_request_t* out) {
    for(size_t i = 0; i < header_count; i++) {
        out->request_headers[i] = request_headers[i];      
    }
    out->url = strdup(url);
    out->request_type = request_type;
    out->header_count = header_count;
    out->request_body = strdup(request_body);
    out->body_length = body_length;
    out->origin = strdup(origin);
    return 1;
}

int parse_http_request_body(const char* body, size_t body_size, queue_item_t* out) {
    json_error_t error;
    json_t* json = json_loads(body, 0, &error);

    create_guid(strdup(json_string_value(json_object_get(json, "guid"))), &out->guid);
    out->command = strdup(json_string_value(json_object_get(json, "command")));
    out->source = strdup(json_string_value(json_object_get(json, "source")));
    out->dest = strdup(json_string_value(json_object_get(json, "dest")));
    out->input_file_name = strdup(json_string_value(json_object_get(json, "input_file_name")));
    out->output_file_name = strdup(json_string_value(json_object_get(json, "output_file_name")));
    out->item_type = (int) json_number_value(json_object_get(json, "item_type"));
    // out->progress = (int) json_number_value(json_object_get(json, "progress"));
    return 1;
}