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
            "example-protocol",
            server_callback_websockets,
            0,
            EXAMPLE_RX_BUFFER_BYTES,
        },
        {NULL, NULL, 0, 0} /* terminator */
};

int server_callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    switch (reason)
    {
    case LWS_CALLBACK_HTTP:
        lws_serve_http_file(wsi, "example.html", "text/html", NULL, 0);
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