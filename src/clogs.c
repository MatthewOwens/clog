#include "clogs.h"
#include <stdio.h>
#include <threads.h>

#define CLOGS_QUEUE_MAX 64

struct clogs
{
	// queues
	char* 				msg_q[CLOGS_QUEUE_MAX+1];
	enum clogs_level	lvl_q[CLOGS_QUEUE_MAX+1];
	char* 				fnc_q[CLOGS_QUEUE_MAX+1];
	// trackers
	int first, last, count;
	// concurrency
	//mtx_t put_mtx;
	//mtx_t pop_mtx;

	FILE* logfile;
};

static char* levels[] = 
{
	"[info]",
	"[warn]",
	"[error]"
};

static FILE* streams[3];

static char* colors[] = {
	"",
	ANSI_COLOR_YELLOW,
	ANSI_COLOR_RED
};

static struct clogs clog;

void clog_put(enum clogs_level lvl, const char* fnc, const char* msg)
{
	//mtx_lock(&clog.put_mtx);

	if(clog.count < CLOGS_QUEUE_MAX - 1) {
		clog.last = (clog.last +1) % CLOGS_QUEUE_MAX;

		printf(__FUNCTION__);
		printf("\n\t - msg is '%s'\n", msg);
		printf("\t - clog.last is %d\n", clog.last);

		clog.msg_q[clog.last] = msg;
		clog.lvl_q[clog.last] = lvl;
		clog.fnc_q[clog.last] = fnc;
		clog.count++;
	} else {
		fprintf(stderr, levels[CLOGS_WARN]);
		fprintf(stderr, ": clogs q full!\n");
	}

	//mtx_unlock(&clog.put_mtx);
}

void stream_print()
{
	int i = clog.first;
	char* msg[100];

	snprintf(msg, 100, "%s%s (%s) '%s'%s\n",
			colors[clog.lvl_q[i]],
			levels[clog.lvl_q[i]],
			clog.fnc_q[i],
			clog.msg_q[i],
			ANSI_COLOR_RESET);
	fprintf(streams[clog.lvl_q[i]], msg);
}

void file_print()
{
	int i = clog.first;
	char* msg[100];

	snprintf(msg, 100, "%s (%s) '%s'\n",
			levels[clog.lvl_q[i]],
			clog.fnc_q[i],
			clog.msg_q[i]);
}

void clogs_pop()
{
	//mtx_lock(&clog.pop_mtx);

	// printing clog
	if(clog.count > 0){
		stream_print();
		if(clog.logfile != NULL) {
			file_print();
		}
	}

	// popping clog
	clog.first = (clog.first + 1) % CLOGS_QUEUE_MAX;
	clog.count--;

	//mtx_unlock(&clog.pop_mtx);
}

void clogs_init(const char* logfile)
{
	if(logfile != NULL) {
		clog.logfile = fopen(logfile, "a+");
	}

	clog.first = 0;
	clog.last = CLOGS_QUEUE_MAX - 1;
	clog.count = 0;

	streams[0] = stdout;
	streams[1] = stdout;
	streams[2] = stderr;

	//mtx_init(&clog.put_mtx, mtx_plain);
	//mtx_init(&clog.pop_mtx, mtx_plain);
}

void clogs_update()
{
	while(clog.count != 0) {
		clogs_pop();
	}
}

void clogs_close()
{
	// ensuring that our queue is all parsed
	clogs_update();

	if(clog.logfile != NULL){
		fclose(clog.logfile);
	}

	//mtx_destroy(&clog.put_mtx);
	//mtx_destroy(&clog.pop_mtx);
}

void clogs_put(enum clogs_level l, const char* func, const char* msg)
{
	clog_put(l, func, msg);
}
