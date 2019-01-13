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
queue_t q;
queue_item_t item; 

void* start_server(void* ptr);
void* start_client(void* ptr);

int main(int argc, char *argv[])
{
	int err = 0;
	create_new_guid(&guid);
	printf("Guid = %s \n", guid.value);

	create_queue(&q, 10L);
	create_item("A", "B", "C", 0, &item);
	printf("Command = %s \n", item.command);
	push_item(&q, &item);

	queue_item_t* item2 = pop_item(&q);
	printf("Command = %s \n", item2->command);

	err = pthread_create(&(tid[0]), NULL, &start_server, NULL);
	if (err != 0) {
		printf("\n can't create thread :[%s] \n", strerror(err));
		exit(1);
	}
	else {
		printf("Server Thread created successfully \n");
	}

	err = pthread_create(&(tid[1]), NULL, &start_client, NULL);
	if (err != 0) {
		printf("\n can't create thread :[%s] \n", strerror(err));
		exit(1);
	}
	else {
		printf("Client Thread created successfully \n");
	}

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	return EXIT_SUCCESS;
}

void* start_server(void* ptr) {
	printf("Starting Server... \n");
	start_websocket_server();

	return NULL;
}

void* start_client(void* ptr) {
	printf("Starting Client... \n");

	return NULL;
}