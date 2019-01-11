#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <websockets.h>
#include <guid.h>
#include <queue.h>
#include <queue_item.h>

guid_t guid;
pthread_t tid[2];

void start_server(void);
void start_client(void);

int main(int argc, char *argv[])
{
	int err = 0;
	create_new_guid(&guid);
	printf("Guid = %s \n", guid.value);



	err = pthread_create(&(tid[0]), NULL, &start_server, NULL);
	if (err != 0) {
		printf("\n can't create thread :[%s] \n", strerror(err));
		exit(1);
	}
	else {
		printf("\n Thread created successfully \n");
	}

	err = pthread_create(&(tid[1]), NULL, &start_client, NULL);
	if (err != 0) {
		printf("\n can't create thread :[%s] \n", strerror(err));
		exit(1);
	}
	else {
		printf("\n Thread created successfully \n");
	}

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	return EXIT_SUCCESS;
}

void start_server() {
	printf("Starting Server... \n");
	start_websocket_server();
}

void start_client() {
	printf("Starting Client... \n");
}