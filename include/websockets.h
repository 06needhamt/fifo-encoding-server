#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include <libwebsockets.h>

#define EXAMPLE_RX_BUFFER_BYTES (10)

/*
 * Unlike ws, http is a stateless protocol.  This pss only exists for the
 * duration of a single http transaction.  With http/1.1 keep-alive and http/2,
 * that is unrelated to (shorter than) the lifetime of the network connection.
 */
struct pss {
	struct lws_spa *spa;
};

static int interrupted;

static const char * const param_names[] = {
	"text1",
	"send",
};

enum enum_param_names {
	EPN_TEXT1,
	EPN_SEND,
};

struct payload
{
    unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
    size_t len;
} received_payload;

enum protocols_enum
{
    PROTOCOL_HTTP = 0,
    PROTOCOL_FIFO,
    PROTOCOL_COUNT
};

/* default mount serves the URL space from ./mount-origin */

static const struct lws_http_mount mount = {
	/* .mount_next */	       NULL,		/* linked-list "next" */
	/* .mountpoint */		"/",		/* mountpoint URL */
	/* .origin */		"./mount-origin",	/* serve from dir */
	/* .def */			"index.html",	/* default filename */
	/* .protocol */			NULL,
	/* .cgienv */			NULL,
	/* .extra_mimetypes */		NULL,
	/* .interpret */		NULL,
	/* .cgi_timeout */		0,
	/* .cache_max_age */		0,
	/* .auth_mask */		0,
	/* .cache_reusable */		0,
	/* .cache_revalidate */		0,
	/* .cache_intermediaries */	0,
	/* .origin_protocol */		LWSMPRO_FILE,	/* files in a dir */
	/* .mountpoint_len */		1,		/* char count */
	/* .basic_auth_login_file */	NULL,
};
int server_callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
int server_callback_websockets(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

int start_websocket_server();
