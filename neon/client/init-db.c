/*
 * NEON - A simple project revision tracker
 *
 * Copyright (c) 2023, 2024 Ariston Lorenzo.
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
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
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

#include "cache.h"
#include "neon.h"

#include <arpa/inet.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef lint
static const char copyright[] =
"@(#) Copyright (c) 2021, 2022, 2023, 2024\n\
	Ariston Lorenzo. All rights reserved.";
#endif /* not lint */

/*
 * Create the .projdir filesystem/database
 */

int init_cache(char *path) {
	FILE *cache_file = fopen(path, "w");
	struct cache_header header;

	if (cache_file == NULL) {
		printf("Unable to create cache file.\n");
		exit(1);
	}

	memset(&header, 0, sizeof(header));
	header.sig = htonl(CACHE_SIGNATURE);
	header.ver = htonl(1);
	header.ent = htonl(0);

	fwrite(&header, sizeof(header), 1, cache_file);

	fclose(cache_file);

	return 0;
}

int init_db(void)
{
	char *prefix = getenv("HOME");
	char *config_template_path = malloc(strlen(prefix) + 32);

	/* template is located at prefix/share/neon/templates/config */
	sprintf(config_template_path, "%s/share/neon/templates/config", prefix);

	FILE *config_template = fopen(config_template_path, "r");

	char buffer[1024];
	size_t bytes_read;

	char cwd[PATH_MAX];

	if (mkdir(".projdir", 0700) < 0) {
		printf("Unable to create .projdir/.\n");
		exit(1);
	}

	if (config_template == NULL) {
		printf("Unable to access repository templates!\n");
		printf("config_template_path: %s\n", config_template_path);
		exit(1);
	}

	FILE *config_file = fopen(".projdir/config", "w");

	/* Check if we can write to the config file */
	if (config_file == NULL) {
		printf("Unable to create config file.\n");
		exit(1);
	}

	while ((bytes_read = fread(buffer, 1, sizeof(buffer), config_template)) > 0) {
		fwrite(buffer, 1, bytes_read, config_file);
	}

	fclose(config_template);
	fclose(config_file);
	
	init_cache(".projdir/cache");

	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("Initialized new Neon project in %s/.projdir.\n",
			cwd);
	} else {
		printf("Unable to get current working directory.\n");
		exit(1);
	}

	return 0;
}
