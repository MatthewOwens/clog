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

#include <stdio.h>

struct clogs;

enum clogs_level
{
	CLOGS_INFO,
	CLOGS_WARN,
	CLOGS_ERR
};

void init_clogs(const char* logfile);
void update_clogs();
void close_clogs();
void clog(enum clogs_level l, static char* func, static char* msg);
