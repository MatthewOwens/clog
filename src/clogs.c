#include "clogs.h"
#include <stdio.h>
#include <threads.h>

#define CLOGS_QUEUE_MAX 64

struct clogs
{
	// queues
	char* 				msg_q[CLOGS_QUEUE_MAX];
	enum clogs_level	lvl_q[CLOGS_QUEUE_MAX];
	char* 				fnc_q[CLOGS_QUEUE_MAX];
	// trackers
	int first, last, count;
	// concurrency
	mtx_t* put_mtx, pop_mtx;

	FILE logfile;
};

static char* levels[] = 
{
	"[info] ",
	"[warn] ",
	"[error] "
};

static FILE* streams[] =
{
	stdout,
	stdout,
	stderr
};

static char* colors[] = {
	"",
	ANSI_COLOR_YELLOW,
	ANSI_COLOR_RED
}

static struct clogs clog;

void put_clog(static char* lvl, static char* fnc, static char* msg)
{
	mtx_lock(clog.put_mtx);

	if(clog.count < CLOGS_QUEUE_MAX - 1) {
		clog.last = (clog.last +1) % CLOGS_QUEUE_MAX;
		clog.msg_q[clog.last] = msg;
		clog.lvl_q[clog.last] = lvl;
		clog.fnc_q[clog.last] = fnc;
		clog.count++;
	} else {
		fprintf(stderr, levels[CLOGS_WARN] ": clogs q full!\n");
	}

	mtx_unlock(clog.put_mtx);
}

void pop_clog()
{
	mtx_lock(clog.pop_mtx);

	int i = clog.first;
	// printing clog
	if(clog.count > 0){
		fprintf(streams[clogs.lvl_q[i]], colors[clogs.lvl_q[i]] levels[clogs.lvl_q[i]] "(" clog.msg_q[fnc_q[i]] ")" clog.msg_q[i] ANSI_COLOR_RESET);
		if(clog.logfile != NULL) {
			fprintf(clog.logfile, levels[clogs.lvl_q[i]] "(" clog.msg_q[fnc_q[i]] ")" clog.msg_q[i]);
		}
	}

	// popping clog
	clog.first = (clog.first + 1) % CLOGS_QUEUE_MAX;
	clog.count--;

	mtx_unlock(clog.pop_mtx);
}

void init_clogs(const char* logfile)
{
	if(logfile != NULL) {
		clog.logfile = fopen(logfile, "a+");
	}

	clog.first = 0;
	clog.last = CLOGS_QUEUE_MAX;
	clog.count = 0;

	mtx_init(clog.put_mtx, mtx_plain);
	mtx_init(clog.pop_mtx, mtx_plain);
}

void update_clogs()
{
	while(clog.count != 0) {
		pop_clog();
	}
}

void close_clogs()
{
	// ensuring that our queue is all parsed
	udpate_clogs();

	if(clog.logfile != NULL){
		fclose(clog.logfile);
	}

	mtx_destroy(clog.put_mtx);
	mtx_destroy(clog.pop_mtx);
}

void clog(enum clogs_level l, static char* func, static char* msg)
{
	put_clog(l, func, msg);
}
