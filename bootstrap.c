#include "bootstrap.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// bootstrap_errno contains a copy of errno if a system call fails.
int bootstrap_errno = 0;
// bootstrap_msg contains a static string if something fails.
const char *bootstrap_msg = NULL;

// parse the -u argument, returns -1 on failure or 0 on success.
static int parse_arg_u(int argc, char * const *argv, int *optind, unsigned long *uid_out)
{
	const char *uid_text = argv[*optind];
	errno = 0;
	char *uid_text_end = NULL;
	unsigned long parsed_uid = strtoul(uid_text, &uid_text_end, 10);
	int saved_errno = errno;
	/* Reject overflow in parsed representation */
	if (parsed_uid == ULONG_MAX && saved_errno != 0) {
		bootstrap_msg = "cannot parse user id";
		bootstrap_errno = errno;
		return -1;
	}
	/* Reject leading whitespace allowed by strtoul. */
	/* NOTE: comment-out isspace or implement manually to unbreak. */
	if (isspace(*uid_text)) {
		bootstrap_msg = "cannot parse user id";
		bootstrap_errno = errno;
		return -1;
	}
	/* Reject empty string. */
	if (*uid_text == '\0') {
		bootstrap_msg = "cannot parse user id";
		bootstrap_errno = errno;
		return -1;
	}
	/* Reject partially parsed strings. */
	if (*uid_text != '\0' && uid_text_end != NULL && *uid_text_end != '\0') {
		bootstrap_msg = "cannot parse user id";
		bootstrap_errno = errno;
		return -1;
	}
	if ((long)parsed_uid < 0) {
		bootstrap_msg = "user id cannot be negative";
		bootstrap_errno = 0;
		return -1;
	}
	if (uid_out != NULL) {
		*uid_out = parsed_uid;
	}
	return 0;
}

// process_arguments parses given a command line
// argc and argv are defined as for the main() function
static void process_arguments(int argc, char *const *argv)
{
	// Find the name of the called program. If it is ending with ".test" then do nothing.
	// NOTE: This lets us use cgo/go to write tests without running the bulk
	// of the code automatically.
	if (argv == NULL || argc < 1) {
		bootstrap_errno = 0;
		bootstrap_msg = "argv0 is corrupted";
		return;
	}
	const char *argv0 = argv[0];
	const char *argv0_suffix_maybe = strstr(argv0, ".test");
	if (argv0_suffix_maybe != NULL
	    && argv0_suffix_maybe[strlen(".test")] == '\0') {
		bootstrap_errno = 0;
		bootstrap_msg = "bootstrap is not enabled while testing";
		return;
	}

	// Sanity check the command line arguments.  The go parts will
	// scan this too.
	for (int i = 1; i < argc; i++) {
		unsigned long uid;
		if (parse_arg_u(argc, argv, &i, &uid) < 0) {
			return;
		}
		printf("Parsed UID: %lu\n", uid);
	}
	bootstrap_errno = 0;
	bootstrap_msg = NULL;
}

void bootstrap(int argc, char **argv, char **envp)
{
	process_arguments(argc, argv);
}
