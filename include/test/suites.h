/* SPDX-License-Identifier: GPL-2.0 */
/*
 * (C) Copyright 2015
 * Joe Hershberger, National Instruments, joe.hershberger@ni.com
 */

#ifndef __TEST_SUITES_H__
#define __TEST_SUITES_H__

struct cmd_tbl;
struct unit_test;
struct unit_test_state;

/* 'command' functions normally called do_xxx where xxx is the command name */
typedef int (*ut_cmd_func)(struct unit_test_state *uts, struct cmd_tbl *cmd,
			   int flags, int argc, char *const argv[]);

/**
 * cmd_ut_category() - Run a category of unit tests
 *
 * @uts: Unit-test state, which must be ready for use, i.e. ut_init_state()
 *	has been called. The caller is responsible for calling
 *	ut_uninit_state() after this function returns
 * @name:	Category name
 * @prefix:	Prefix of test name
 * @tests:	List of tests to run
 * @n_ents:	Number of tests in @tests
 * @argc:	Argument count provided. Must be >= 1. If this is 1 then all
 *		tests are run, otherwise only the one named @argv[1] is run.
 * @argv:	Arguments: argv[1] is the test to run (if @argc >= 2)
 * Return: 0 if OK, CMD_RET_FAILURE on failure
 */
int cmd_ut_category(struct unit_test_state *uts, const char *name,
		    const char *prefix, struct unit_test *tests, int n_ents,
		    int argc, char *const argv[]);

int do_ut_bootstd(struct unit_test_state *uts, struct cmd_tbl *cmdtp, int flag,
		  int argc, char *const argv[]);
int do_ut_optee(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[]);
int do_ut_overlay(struct unit_test_state *uts, struct cmd_tbl *cmdtp, int flag,
		  int argc, char *const argv[]);

#endif /* __TEST_SUITES_H__ */
