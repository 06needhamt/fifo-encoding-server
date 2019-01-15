#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include <websockets.h>
#include <guid.h>
#include <queue.h>
#include <queue_item.h>
#include <io.h>

pthread_t tid[2];
int err = 1;

void* start_server(void* ptr);
void* start_main_thread(void* ptr);
int pre_start_tests(void);

void* main_status;
void* server_status;

int main(int argc, char *argv[])
{
	main_status = malloc(sizeof(int));
	server_status = malloc(sizeof(int));

	err = pthread_create(&(tid[0]), NULL, &start_main_thread, NULL);
	if (err != 0) {
		printf("\n can't create thread :[%s] \n", strerror(err));
		exit(1);
	}
	else {
		printf("Main Thread created successfully \n");
	}

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	int status = (*(int*) server_status) & *((int*) main_status); 

	free(server_status);
	free(main_status);

	if(!status)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

void* start_server(void* ptr) {
	printf("Starting Server... \n");
	start_websocket_server();

	return NULL;
}

void* start_main_thread(void* ptr) {
	printf("Starting Main Thread... \n");
	printf("Running Pre Start Tests \n");

	if(!pre_start_tests())
		return false;

	printf("Pre Start Tests Passed! \n");

	err = pthread_create(&(tid[1]), NULL, &start_server, NULL);
	if (err != 0) {
		printf("\n can't create thread :[%s] \n", strerror(err));
		exit(1);
	}
	else {
		printf("Server Thread created successfully \n");
	}

	return true;
}

int pre_start_tests() {
	guid_t guid;
	queue_t q;
	queue_item_t item; 

	FILE* file;
	
	create_new_guid(&guid);
	printf("Guid = %s \n", guid.value);

	create_queue(&q, 10L);
	create_item("A\0", "B\0", "C\0", "test.mp4", 0, &item);
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
	
	item = pop_item(&q);
	
	//push_item(&q, NULL);
	
	printf("Command = %s \n", item.command);
	
	remove("queue.json");
	return true;
}