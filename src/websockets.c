#include <websockets.h>

struct lws_protocols protocols[] =
    {
        /* The first protocol must always be the HTTP handler */
        {
            "http-only",   /* name */
            server_callback_http, /* callback */
            0,             /* No per session data. */
            0,             /* max frame size / rx buffer */
        },
        {
            "fifo-protocol",
            server_callback_websockets,
            0,
            EXAMPLE_RX_BUFFER_BYTES,
        },
        {NULL, NULL, 0, 0} /* terminator */
};

int server_callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    struct pss *pss = (struct pss *)user;
	uint8_t buf[LWS_PRE + 0x8000], *start = &buf[LWS_PRE],
		*p = start, *end = &buf[sizeof(buf) - 1];
	int n;
    
    switch (reason)
    {
    case LWS_CALLBACK_HTTP:
        lws_serve_http_file(wsi, "example.html", "text/html", NULL, 0);
        break;
    case LWS_CALLBACK_HTTP_BODY:

		/* create the POST argument parser if not already existing */

		if (!pss->spa) {
			pss->spa = lws_spa_create(wsi, param_names,
					LWS_ARRAY_SIZE(param_names), 1024,
					NULL, NULL); /* no file upload */
			if (!pss->spa)
				return -1;
		}

		/* let it parse the POST data */

		if (lws_spa_process(pss->spa, in, (int)len))
			return -1;
		break;

	case LWS_CALLBACK_HTTP_BODY_COMPLETION:

		/* inform the spa no more payload data coming */

		lws_spa_finalize(pss->spa);

		/* we just dump the decoded things to the log */

		for (n = 0; n < (int)LWS_ARRAY_SIZE(param_names); n++) {
			if (!lws_spa_get_string(pss->spa, n))
				lwsl_user("%s: undefined\n", param_names[n]);
			else
				lwsl_user("%s: (len %d) '%s'\n",
				    param_names[n],
				    lws_spa_get_length(pss->spa, n),
				    lws_spa_get_string(pss->spa, n));
		}

		/*
		 * Our response is to redirect to a static page.  We could
		 * have generated a dynamic html page here instead.
		 */

		if (lws_http_redirect(wsi, HTTP_STATUS_MOVED_PERMANENTLY,
				      (unsigned char *)"after-form1.html",
				      16, &p, end) < 0)
			return -1;
		break;
    default:
        break;
    }

    return 0;
}

int server_callback_websockets(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    switch (reason)
    {
    case LWS_CALLBACK_RECEIVE:
        memcpy(&received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], in, len);
        received_payload.len = len;
        lws_callback_on_writable_all_protocol(lws_get_context(wsi), lws_get_protocol(wsi));
        break;

    case LWS_CALLBACK_SERVER_WRITEABLE:
        lws_write(wsi, &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], received_payload.len, LWS_WRITE_TEXT);
        break;

    default:
        break;
    }

    return 0;
}

int start_websocket_server()
{
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = 8000;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;

    struct lws_context *context = lws_create_context(&info);

    while (true)
    {
        lws_service(context, /* timeout_ms = */ 1000000);
    }

    lws_context_destroy(context);

    return 0;
}