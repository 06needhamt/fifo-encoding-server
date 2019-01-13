#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <websockets.h>
#include <guid.h>
#include <queue.h>
#include <queue_item.h>
#include <io.h>

guid_t guid;
pthread_t tid[2];
queue_t q;
queue_item_t item; 

FILE* file;

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

	file = fopen("queue.json", "wb");

	if(file == NULL)
    {
        printf("Error opening file \n");
        exit(1);
    }

	write_queue(file, &q);

	fclose(file);

	memset(&q, 0, sizeof(queue_t));
	memset(&item, 0, sizeof(queue_item_t));

	file = fopen("queue.json", "rb");

	if(file == NULL)
    {
        printf("Error opening file \n");
        exit(1);
    }

	read_queue(file, &q);

	fclose(file);
	
	pop_item(&q, &item);
	printf("Command = %s \n", item.command);

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