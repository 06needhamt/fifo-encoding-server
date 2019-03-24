#include <threads.h>

int create_thread_pool(int count, thread_pool_t* out) {
    out->pool = (thread_t*) malloc(sizeof(thread_t) * count);
    create_new_guid(&out->id);
    out->thread_count = count;
    out->idle_count = count;
    out->busy_count = 0;

    for(int i = 0; i < count; i++) {
		out->pool[i].data = (thread_data_t*) malloc(sizeof(thread_data_t));

		int err = create_thread_data(i, out->pool[i].data);
		if (err != 0) {
		    printf("can't create thread data \n");
		    return 0;
	    }
        err = pthread_create(&(out->pool[i].handle), NULL, &poll_thread, out->pool[i].data);
        if (err != 0) {
		    printf("can't create thread :[%s] \n", strerror(err));
		    return 0;
	    }
	    else {
		    printf("Thread %i created successfully in pool %s \n", i, out->id.value);
        }
        out->pool[i].busy = 0;
        create_new_guid(&out->pool[i].id);
    }
    return 1;
}

int destroy_thread_pool(thread_pool_t* in) {
    busy = (char) 0xFF;
    for(int i = 0; i < in->thread_count; i++) {
        pthread_join(in->pool[i].handle, NULL);
        free((void*) (in->pool[i].handle));
    }
    free(in);
    return 1;
}

int create_thread_data(int tid, thread_data_t* out) {
	out->tid = tid;
	out->tra_ctx = (TranscodingContext*) malloc(sizeof(TranscodingContext));

	return 0;
}
