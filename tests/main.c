#include <stdio.h>
#include "clogs.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define THREAD_MAX 15
pthread_t threads[THREAD_MAX];
int tid[THREAD_MAX];

void foo(void* data)
{
	int* thrdn = (int *)data;
	CLOG("thread %d", *thrdn);
	CLOG_WARN("thread %d", *thrdn);
	CLOG_ERR("thread %d", *thrdn);
}

int main()
{
	clogs_init("/tmp/clogs.out");
	for(int i = 0; i < THREAD_MAX; ++i){
		tid[i] = i;
		pthread_create(&threads[i], NULL, foo, &tid[i]);
	}

	for(int i = 0; i < THREAD_MAX; ++i){
		pthread_join(threads[i], NULL);
	}

	clogs_close();
	return 0;
}
