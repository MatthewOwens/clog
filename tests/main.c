#include <stdio.h>
#include "clogs.h"
#include "threads.h"
#include <string.h>
#include <stdlib.h>
#define THREAD_MAX 15

thrd_t threads[THREAD_MAX];

void foo(void* data)
{
	int* thrd = (int *)data;
	CLOG("thread %d", *thrd);
	CLOG_WARN("thread %d", *thrd);
	CLOG_ERR("thread %d", *thrd);
}

int main()
{
	clogs_init("/tmp/clogs.out");
	//for(int i = 0; i < THREAD_MAX; ++i){
	//	thrd_create(&threads[i], foo, &i);
	//}

	//for(int i = 0; i < THREAD_MAX; ++i){
	//	thrd_join(threads[i], NULL);
	//}
	int i = 5;
	foo(&i);
	//foo(&i);

	clogs_close();
	return 0;
}

