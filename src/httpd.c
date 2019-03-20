#include <httpd.h>

int send_page(struct MHD_Connection* connection, const char* page) {
	int ret;
	struct MHD_Response* response;

	response = MHD_create_response_from_buffer(strlen(page), (void*)page, MHD_RESPMEM_PERSISTENT);
	if (!response)
		return MHD_NO;

	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	return ret;
}

int answer_to_connection(void* cls, struct MHD_Connection* connection, const char* url, const char* method,
								const char* version, const char* upload_data, size_t* upload_data_size, void** con_cls) {
	if (*con_cls == NULL)
	{
		connection_info_t* con_info;

		con_info = malloc(sizeof(connection_info_t));
		if (NULL == con_info)
			return MHD_NO;
		con_info->answerstring = NULL;

		if (strcmp(method, "POST") == 0)
		{
			con_info->connectiontype = POST;
		}
		else
			con_info->connectiontype = GET;

		*con_cls = (void*)con_info;

		return MHD_YES;
	}

	if (strcmp(method, "GET") == 0)
	{
		return send_page(connection, askpage);
	}

	if (strcmp(method, "POST") == 0)
	{
		connection_info_t* con_info = *con_cls;
		con_info->answerstring = malloc(*upload_data_size);
		strcpy(con_info->answerstring, upload_data);
		
		if (con_info->answerstring != NULL) {
			if(item == NULL)
				item = malloc(sizeof(queue_item_t));
			else
				memset(item, 0x00, sizeof(queue_item_t));
			
			parse_http_request_body(con_info->answerstring, *upload_data_size, item);
			push_item(current_queue, item);
			return send_page(connection, con_info->answerstring);
		}
	}

	return send_page(connection, errorpage);
}

void request_completed(void* cls, struct MHD_Connection* connection,
				void** con_cls, enum MHD_RequestTerminationCode toe)
{
	connection_info_t* con_info = *con_cls;

	if (con_info == NULL)
		return;

	free(con_info);
	*con_cls = NULL;
}

int iterate_post(void* coninfo_cls, enum MHD_ValueKind kind, const char* key,
				const char* filename, const char* content_type,
				const char* transfer_encoding, const char* data, uint64_t off, size_t size) {
	
	connection_info_t* con_info = coninfo_cls;

	if ((size > 0) && (size <= MAXBODYSIZE))
	{
		char* body;
		body = malloc(MAXRESPONSESIZE);
		if (!body)
			return MHD_NO;

		snprintf(body, MAXRESPONSESIZE, greetingpage, data);
		con_info->answerstring = body;
		free(body);
	}
	else {
		con_info->answerstring = NULL;
		printf("NULL Request Ignored");
	}
	
	return MHD_YES;
}

int start_http_server()
{
  struct MHD_Daemon* daemon;

  daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
							 &answer_to_connection, NULL,
							 MHD_OPTION_NOTIFY_COMPLETED, request_completed,
							 NULL, MHD_OPTION_END);
	if (daemon == NULL)
		return EXIT_FAILURE;

	getchar();

	MHD_stop_daemon(daemon);

	return EXIT_SUCCESS;
}
