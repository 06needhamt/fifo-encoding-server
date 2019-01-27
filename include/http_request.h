#pragma once

#include <globals.h>

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
	const http_request_header_t* request_headers;
	http_request_type_t request_type;
	const char* request_body;
	size_t body_length;
	const char* origin;
} http_request_t;

create_http_request_header(const char* header, const char* value);
create_http_request(const http_request_header_t* request_headers, http_request_type,
			const char* request_body, size_t body_length, const char* origin);