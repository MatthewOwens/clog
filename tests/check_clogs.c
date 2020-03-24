#include "check_clogs.h"
#include <stdio.h>
#include "clogs.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <check.h>

#define LOGFILE "log.txt"
#define BUFSIZE 2048
#define THREAD_MAX 9
#define THREAD_MSG_COUNT 3
pthread_t threads[THREAD_MAX];
int tid[THREAD_MAX];

void *foo(void* data)
{
	int* thrdn = (int *)data;
	CLOG("thread %d", *thrdn);
	CLOG_WARN("thread %d", *thrdn);
	CLOG_ERR("thread %d", *thrdn);
	return NULL;
}

static void setup()
{
	clogs_init(LOGFILE);
	for(int i = 0; i < THREAD_MAX; ++i){
		tid[i] = i;
		pthread_create(&threads[i], NULL, foo, &tid[i]);
	}

	for(int i = 0; i < THREAD_MAX; ++i){
		pthread_join(threads[i], NULL);
	}
	clogs_close();
}

static void teardown()
{
}

START_TEST(test_clogs)
{
	// required for quick char conversions down the line
	ck_assert_int_le(THREAD_MAX, 9);

	char buf[BUFSIZE];		// _plenty_ of room
	int occurances[THREAD_MAX] = { 0 };
	FILE* logfile = fopen(LOGFILE, "r");
	int n_read = -1;
	int n_newlines = 0;

	ck_assert_ptr_nonnull(logfile);
	n_read = fread(&buf[0], 1, BUFSIZE, logfile);
	//n_read = fread(&buf, BUFSIZE, 1, logfile);
	ck_assert_int_ne(feof(logfile), 0);
	ck_assert_int_ne(n_read, -1);
	ck_assert_int_lt(n_read, BUFSIZE);

	printf("read %d bytes from %s\n", n_read, LOGFILE);

	rewind(logfile);

	// parsing our file
	char c;
	int tmp;
	for(int i = 0; i < n_read; ++i) {
		c = buf[i];

		if(c == '\n'){
			n_newlines++;
		}


		// have we printed a number?
		tmp = c - '0';
		if(tmp >= 0 && tmp < THREAD_MAX){
			occurances[tmp]++;
		}
	}

	// checking that we have the expected number of messages
	ck_assert_int_eq(n_newlines, (THREAD_MAX * THREAD_MSG_COUNT));

	/*
	 * checking that we have the expected number of occurences for each
	 * thread id
	 */
	for(int i = 0; i < THREAD_MAX; ++i){
		ck_assert_int_eq(occurances[i], THREAD_MSG_COUNT);
	}
}
END_TEST

Suite *clogs_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("clogs suite");
	tc = tcase_create("clogs");

	tcase_add_unchecked_fixture(tc, setup, teardown);
	tcase_add_test(tc, test_clogs);

	suite_add_tcase(s, tc);
	return s;
}
