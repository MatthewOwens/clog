#pragma once

#ifdef CLOG_NO_COLOR
	#define ANSI_COLOR_RED     ""
	#define ANSI_COLOR_GREEN   ""
	#define ANSI_COLOR_YELLOW  ""
	#define ANSI_COLOR_BLUE    ""
	#define ANSI_COLOR_MAGENTA ""
	#define ANSI_COLOR_CYAN    ""
	#define ANSI_COLOR_RESET   ""
#else
	#define ANSI_COLOR_RED     "\x1b[31m"
	#define ANSI_COLOR_GREEN   "\x1b[32m"
	#define ANSI_COLOR_YELLOW  "\x1b[33m"
	#define ANSI_COLOR_BLUE    "\x1b[34m"
	#define ANSI_COLOR_MAGENTA "\x1b[35m"
	#define ANSI_COLOR_CYAN    "\x1b[36m"
	#define ANSI_COLOR_RESET   "\x1b[0m"
#endif

#define CLOG(msg) clogs_put(CLOGS_INFO, __FUNCTION__, msg)
#define CLOG_WARN(msg) clogs_put(CLOGS_WARN, __FUNCTION__, msg)
#define CLOG_ERR(msg) clogs_put(CLOGS_ERR, __FUNCTION__, msg)

#include <stdio.h>

struct clogs;

enum clogs_level
{
	CLOGS_INFO,
	CLOGS_WARN,
	CLOGS_ERR
};

void clogs_init(const char* logfile);
void clogs_update();
void clogs_close();
void clogs_put(enum clogs_level l, const char* func, const char* msg);
