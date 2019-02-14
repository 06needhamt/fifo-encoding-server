#pragma once

#include <jansson.h>

#include <globals.h>
#include <queue.h>
#include <queue_item.h>

typedef enum http_request_type {
	NEW_JOB = 0x1,
	PING = 0x2,
	PROGRESS_REPORT = 0x04,
	ABORT_JOB = 0x08,
	CTRL = 0x10,
	HALT = 0x20,
} http_request_type_t;

typedef enum http_request_mode {
	GET = 0x1,
	POST = 0x2,
} http_request_mode_t;

typedef struct http_request_header {
	const char* header;
	const char* value;
} http_request_header_t;

typedef struct http_request {
	http_request_header_t* const request_headers;
	const char* url;
	http_request_type_t request_type;
	size_t header_count;
	const char* request_body;
	size_t body_length;
	const char* origin;
} http_request_t;

int create_http_request_header(const char* header, const char* value, http_request_header_t*  out);
int create_http_request(const http_request_header_t* request_headers, const char* url, http_request_type_t request_type,
			size_t header_count, const char* request_body, size_t body_length, const char* origin, http_request_t* out);
int send_http_request(http_request_mode_t mode, const char* url, http_request_t* payload);
int parse_http_request_body(const char* body, size_t body_size, queue_item_t* out);