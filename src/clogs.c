#include "clogs.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef CLOGS_QUEUE_MAX
	#define CLOGS_QUEUE_MAX 64
#endif

#ifndef CLOGS_MSG_MAX
	#define CLOGS_MSG_MAX 80
#endif

struct clogs 
{
	FILE* logfile;
	char* msg_queue[CLOGS_QUEUE_MAX][CLOGS_MSG_MAX];
	int first, last, count;

	FILE* streams[CLOGS_ERR + 1];
	char* colors[CLOGS_ERR + 1];
	char* levels[CLOGS_ERR + 1];
};

static struct clogs c;

void clogs_pop()
{
	if(c.count > 0) {
		if(c.logfile != NULL) { fprintf(c.logfile, c.msg_queue[c.first]); }

		printf(c.msg_queue[c.first]);
		memset(c.msg_queue[c.first], 0, CLOGS_MSG_MAX);
		c.first = (c.first + 1) % CLOGS_QUEUE_MAX;
		c.count--;
	}
}

void clogs_flush()
{
	while(c.count > 0) {
		clogs_pop();
	}
}

void clogs_update() //TODO
{
	clogs_flush();
}

void clogs_put(enum clogs_level l, const char* func, const char* format, ...)
{
	if(c.count >= CLOGS_QUEUE_MAX) {
		fprintf(c.streams[CLOGS_ERR],"%s%s (%s) CLOGS_QUEUE_MAX reached!%s\n",
				c.colors[CLOGS_ERR], c.levels[CLOGS_ERR], __FUNCTION__,
				ANSI_COLOR_RESET);
	} else { 
		char tmp[CLOGS_MSG_MAX] = { '\0' };
		//TODO: this whole block
		c.last = (c.last + 1) % CLOGS_QUEUE_MAX;
		//strcat(c.msg_queue[c.last], 
		snprintf(c.msg_queue[c.last], CLOGS_MSG_MAX, "%s (%s) ",
				c.levels[l], func);
		int len = strlen(c.msg_queue);

		//TODO: colors, level, func
		va_list args;
		va_start(args, format);
		vsnprintf(tmp, CLOGS_MSG_MAX, format, args);
		va_end(args);

		strncat(c.msg_queue[c.last], tmp, CLOGS_MSG_MAX-len);

		c.count++;
	}
}

void clogs_init(const char* logfile)
{
	c.first = 0;
	c.last = CLOGS_QUEUE_MAX - 1;
	c.count = 0;

	c.streams[CLOGS_INFO] = stdout;
	c.streams[CLOGS_WARN] = stdout;
	c.streams[CLOGS_ERR] = stderr;

	c.colors[CLOGS_INFO] = "";
	c.colors[CLOGS_WARN] = ANSI_COLOR_YELLOW;
	c.colors[CLOGS_ERR] = ANSI_COLOR_RED;

	c.levels[CLOGS_INFO] = "[info]";
	c.levels[CLOGS_WARN] = "[warn]";
	c.levels[CLOGS_ERR] = "[error]";

	if(logfile != NULL) {
		c.logfile = fopen(logfile, "w+");
		if(c.logfile == NULL) { CLOG_ERR("can't open logfile %s!", logfile); }
	}

	// ensuring we don't have garbage in the messages
	for(int i = 0; i < CLOGS_QUEUE_MAX; ++i) {
		memset(c.msg_queue[i], 0, CLOGS_MSG_MAX);
	}
}

void clogs_close()
{
	clogs_flush();
	if(c.logfile != NULL) { fclose(c.logfile); }
}
