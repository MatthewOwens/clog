#include <stdio.h>
#include "clogs.h"
#include "threads.h"
#define THREAD_MAX 15

thrd_t threads[THREAD_MAX];

void foo(void* data)
{
	char msg[32] = { '\0' };
	int *i = (int *)data;
	strcpy(msg, "clogs test ");
	//strcpy(msg+11, (*i) - '0');
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
	foo(&i);

	clogs_close();
	return 0;
}

