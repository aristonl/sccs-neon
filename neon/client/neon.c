/*
 * NEON - A simple project revision tracker
 *
 * Copyright (c) 2023, 2024 Ariston Lorenzo. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "neon.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <bits/getopt_core.h>

#ifndef lint
static const char copyright[] =
"@(#) Copyright (c) 2021, 2022, 2023, 2024\n\
	Ariston Lorenzo. All rights reserved.\n";
#endif /* not lint */

/*
 *  neon.c - the front end to the NEON revision system.
 *
 *  This just makes NEON easier to use by wrapping all the separate programs
 *  together through a singular program without having to compile NEON into
 *  one program all-together, removing the UNIX philosophy from the system.
 *  This front-end also makes some of the names more user-friendly and closer
 *  to other version control systems like CVS and Git.
 *
 *  Usage:
 *	neon [-hv] <command> [<args>]
 *
 *  Flags:
 *	-h - Prints the usage. Also works as a command argument.
 *	-v - Prints version and copyright info.
 *
 *  Commands:
 *	add  | write-cache - Adds a new file entry to the project cache.
 *	ls   | read-cache  - Lists all the entries in the cache.
 *	init | init-db     - Initializes the NEON filesystem/database.
 *
 *  Author:
 *	Ariston Lorenzo <me@ariston.dev>
 *	Copyright 2021, 2022, 2023 Ariston Lorenzo.
 */

int version(void) {
	printf("NEON version %s\n\n", NEON_VERSION);
	printf("Copyright (c) 2021, 2022, 2023, 2024 Ariston Lorenzo. All rights resevred.\n");
	return 0;
}

int main(int argc, char *argv[]) {
	/* variable definitions */
	static char usage[] = "Usage: neon [-hv] <command> [<args>]\n";
	int ch;

	/* Option handling */
	while ((ch = getopt(argc, argv, "hv")) != -1) {
		switch (ch) {
		case 'h':
			printf("%s", usage);
			exit(0);
			break;
		case 'v':
			version();
			exit(0);
			break;
		default:
		case '?':
			printf("%s", usage);
			exit(1);
			break;
		}
	}

	/* Check for the command */
	if (strcmp(argv[optind], "add") == 0 || strcmp(argv[optind], "write-cache") == 0) {
		/* Usage: neon add <file> */
		if (argv[optind + 1] == NULL) {
			printf("Usage: neon add <file>\n");
			return 1;
		}
		write_cache(argv[optind + 1]);
	} else if (strcmp(argv[optind], "ls") == 0 || strcmp(argv[optind], "read-cache") == 0) {
		read_cache();
	} else if (strcmp(argv[optind], "init") == 0 || strcmp(argv[optind], "init-db") == 0) {
		init_db();
	} else {
		printf("Unknown command: %s\n", argv[optind]);
		printf("%s", usage);
		return 1;
	}

	return 0;
}
