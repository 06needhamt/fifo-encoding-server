#pragma once

#include <libwebsockets.h>

#define EXAMPLE_RX_BUFFER_BYTES (10)

struct payload
{
    unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
    size_t len;
} received_payload;

enum protocols_enum
{
    PROTOCOL_HTTP = 0,
    PROTOCOL_EXAMPLE,
    PROTOCOL_COUNT
};

int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
int callback_example(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

int start_websocket_server(int argc, char *argv[]);
