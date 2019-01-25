#pragma once

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <globals.h>

#define PORT            5555
#define POSTBUFFERSIZE  1024 * 1024
#define MAXNAMESIZE     255
#define MAXANSWERSIZE   1024 * 1024

#define GET             0
#define POST            1

static const char* askpage = "<html><body>\
					   What's your name, Sir?<br>\
					   <form action=\"/namepost\" method=\"post\">\
					   <input name=\"name\" type=\"text\"\
					   <input type=\"submit\" value=\" Send \"></form>\
					   </body></html>";

static const char* greetingpage = "<html><body><h1>Welcome, %s!</center></h1></body></html>";

static const char* errorpage = "<html><body>This doesn't seem to be right.</body></html>";

typedef struct connection_info
{
	int connectiontype;
	char* answerstring;
	struct MHD_PostProcessor* postprocessor;
} connection_info_t; 

static int send_page(struct MHD_Connection* connection, const char* page);

static int iterate_post (void* coninfo_cls, enum MHD_ValueKind kind, const char* key,
				const char* filename, const char* content_type,
				const char* transfer_encoding, const char* data, uint64_t off, size_t size);

static void request_completed(void* cls, struct MHD_Connection* connection,
				  void** con_cls, enum MHD_RequestTerminationCode toe);

static int answer_to_connection (void* cls, struct MHD_Connection* connection,
				const char* url, const char* method,
				const char* version, const char* upload_data,
				size_t* upload_data_size, void** con_cls);

int start_http_server();
