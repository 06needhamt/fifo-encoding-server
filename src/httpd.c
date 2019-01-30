#include <httpd.h>

static int send_page(struct MHD_Connection* connection, const char* page) {
	int ret;
	struct MHD_Response* response;

	response = MHD_create_response_from_buffer(strlen(page), (void*)page, MHD_RESPMEM_PERSISTENT);
	if (!response)
		return MHD_NO;

	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	return ret;
}

static int answer_to_connection(void* cls, struct MHD_Connection* connection, const char* url, const char* method,
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
			con_info->postprocessor =
				MHD_create_post_processor(connection, POSTBUFFERSIZE, iterate_post, (void*)con_info);

			if (NULL == con_info->postprocessor)
			{
				free(con_info);
				return MHD_NO;
			}

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

		if (*upload_data_size != 0)
		{
			MHD_post_process(con_info->postprocessor, upload_data, *upload_data_size);
			*upload_data_size = 0;

			return MHD_YES;
		}
		else if (con_info->answerstring != NULL)
			return send_page(connection, con_info->answerstring);
	}

	return send_page(connection, errorpage);
}

static void request_completed(void* cls, struct MHD_Connection* connection,
				void** con_cls, enum MHD_RequestTerminationCode toe)
{
	connection_info_t* con_info = *con_cls;

	if (con_info == NULL)
		return;

	if (con_info->connectiontype == POST)
	{
		MHD_destroy_post_processor(con_info->postprocessor);
		if (con_info->answerstring)
			free(con_info->answerstring);
	}

	free(con_info);
	*con_cls = NULL;
}

static int iterate_post_test(void* coninfo_cls, enum MHD_ValueKind kind, const char* key,
				const char* filename, const char* content_type,
				const char* transfer_encoding, const char* data, uint64_t off, size_t size) {

	connection_info_t* con_info = coninfo_cls;

	if (strcmp(key, "name") == 0) {
		if ((size > 0) && (size <= MAXBODYSIZE))
		{
			char* body;
			body = malloc(MAXRESPONSESIZE);
			if (!body)
				return MHD_NO;

			snprintf(body, MAXRESPONSESIZE, greetingpage, data);
			con_info->answerstring = body;
		}
		else {
			con_info->answerstring = NULL;
		}
		return MHD_NO;
	}
	return MHD_YES;
}

static int iterate_post(void* coninfo_cls, enum MHD_ValueKind kind, const char* key,
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
		parse_http_request_body(con_info->answerstring, item);
		push_item(current_queue, item);
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

int send_http_request(http_request_mode_t mode, const char* url, http_request_t* payload) {
	CURLcode res = CURLE_OK;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL,  payload->url);
	if(mode == POST) {
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload->request_body);
	}

	res = curl_easy_perform(curl);
	/* Check for errors */ 
	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
	    curl_easy_cleanup(curl);

    return 1;
}