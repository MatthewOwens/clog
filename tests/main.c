#include <stdio.h>
#include "clogs.h"
#include "threads.h"
#include <string.h>
#include <stdlib.h>
#define THREAD_MAX 15

thrd_t threads[THREAD_MAX];

void foo(void* data)
{
	//char* msg = "this is a test";
	char msg[5];
	int* thrd = (int *)data;
	//printf("thrd is [%d]\n", *thrd);
	snprintf(msg, 5, "%d", *thrd);
	CLOG_ERR(msg);
}

int main()
{
	clogs_init(NULL);
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

