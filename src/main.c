#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <globals.h>
#include <websockets.h>
#include <httpd.h>
#include <guid.h>
#include <queue.h>
#include <queue_item.h>
#include <io.h>
#include <threads.h>

pthread_t tid[3];
int err = 1;

thread_pool_t* pool = NULL;
queue_t* current_queue = NULL;

queue_item_t current_item = {0};

FILE* log_file = NULL;
FILE* data_file = NULL;

void* start_libwebsocket_server(void* ptr);
void* start_httpd_server(void* ptr);
void* start_main_thread(void* ptr);

int pre_start_tests(void);
int live_test(void);
int create_and_allocate_thread_pool(void);
int create_and_allocate_queue(void);
int create_and_open_files(const char* directory, const char* log_path, const char* data_path);
int cleanup_memory(void);

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

	if(!cleanup_memory())
		return EXIT_FAILURE;
	
	int status = (*(int*) server_status) & *((int*) main_status); 

	free(server_status);
	free(main_status);

	if(!status)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

void* start_libwebsocket_server(void* ptr) {
	printf("Starting Websocket Server... \n");
	start_websocket_server();

	return (void*) true;
}

void* start_httpd_server(void* ptr) {
	printf("Starting HTTP Server... \n");
	start_http_server();

	return (void*) true;
}

void* start_main_thread(void* ptr) {
	printf("Starting Main Thread... \n");

	if(!create_and_open_files("data/", "fifoserver.log", "data.json"))
		return (void*) false;

	printf("Running Pre Start Tests \n");

	if(!pre_start_tests())
		return (void*) false;

	printf("Pre Start Tests Passed! \n");

	if(!create_and_allocate_thread_pool())
		return (void*) false;

	if(!create_and_allocate_queue())
		return (void*) false;

	// if(!live_test())
	// 	return (void*) false;
	
	err = pthread_create(&(tid[1]), NULL, &start_libwebsocket_server, NULL);
	if (err != 0) {
		printf("\n can't create thread :[%s] \n", strerror(err));
		exit(2);
	}
	else {
		printf("Websocket Server Thread created successfully \n");
	}

	err = pthread_create(&(tid[2]), NULL, &start_httpd_server, NULL);
	if (err != 0) {
		printf("\n can't create thread :[%s] \n", strerror(err));
		exit(3);
	}
	else {
		printf("HTTP Server Thread created successfully \n");
	}

	return (void*) true;
}

int live_test() {
	queue_item_t item;

	create_item("Transcode", "Source", "Destination", "input.mp4", "output.mp4", 1, &item);
	push_item_temp(current_queue, &item);

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
	create_item("A", "B", "C", "input.mp4", "output.mp4", 0, &item);
	printf("Command = %s \n", item.command);
	push_item_temp(&q, &item);

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
	
	item = pop_item_temp(&q);

	printf("Command = %s \n", item.command);
	remove("queue.json");

	return true;
}

int create_and_allocate_thread_pool() {
	MAX_THREADS = get_nprocs();
	pool = (thread_pool_t*) malloc(sizeof(thread_pool_t));

	create_thread_pool(MAX_THREADS, pool);
	return true;
}

int create_and_allocate_queue() {
	current_queue = (queue_t*) malloc(sizeof(queue_t));
	create_queue(current_queue, QUEUE_CAPACITY);
	return true;
}

int create_and_open_files(const char* directory, const char* log_path, const char* data_path){
	struct stat st = {0};
	char l_path[255];
	char d_path[255];

	log_file_path = malloc(255);
	data_file_path = malloc(255);
	
	if (stat(directory, &st) == -1) {
		mkdir(directory, 0755);
	}

	strcpy(l_path, directory);
	strcat(l_path, log_path);
	printf("log_path %s \n", l_path);

	strcpy(log_file_path, l_path);

	log_file = fopen(l_path, "wb");

	strcpy(d_path, directory);
	strcat(d_path, data_path);
	printf("data_path %s \n", d_path);

	strcpy(data_file_path, d_path);

	data_file = fopen(d_path, "rb");
	
	if(data_file) {
		long size = 0L;
		fseek (data_file, 0, SEEK_END);
		size = ftell(data_file);

		if (size == 0) {
			printf("Data file is empty \n");
		}
		else {
			read_queue(data_file, current_queue);
		}
		fclose(data_file);
	}

	data_file = fopen(d_path, "wb");

	return true;
}

int cleanup_memory() {
	printf("Cleanup Memory Called \n");
	
	write_queue(data_file, current_queue);

	fclose(log_file);
	fclose(data_file);

	free(current_queue);
	free(current_body);
	
	free(log_file_path);
	free(data_file_path);

	destroy_thread_pool(pool);

	return true;
}