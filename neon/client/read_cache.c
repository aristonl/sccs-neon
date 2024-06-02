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
#include "sha1.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int get_number_of_entries(void) {
	FILE *fp;

	struct cache_header header;

	unsigned int num = ntohl(header.ent);

	fp = fopen(".projdir/cache", "r");
	if (fp == NULL) {
		printf("Error opening cache file");
		return 1;
	}

	fread(&header, sizeof(header), 1, fp);

	fclose(fp);

	return num;
}

#ifndef SHA_DIGEST_LENGTH
#define SHA_DIGEST_LENGTH 20
#endif

/*
 * List all entries in the cache.
 */
int read_cache(void) {
	unsigned int entry_count;
	FILE *fp;
	int header_size;
	struct cache_entry entry;
	unsigned int i;

	unsigned char namelen_n;
	unsigned int mode_n;
	unsigned int size_n;

	int entry_size;

	/* Verify the cache header. */
	verify_header();

	entry_count = get_number_of_entries();

	fp = fopen(".projdir/cache", "r");

	if (fp == NULL) {
		printf("Error opening cache file\n");
		return 1;
	}

	/* Skip the cache header */
	header_size = sizeof(struct cache_header);
	printf("read_cache: header_size: %d bytes\n", header_size);
	fseek(fp, header_size, SEEK_SET);

	printf("read_cache: Number of entries: %u\n", entry_count);

	/* Read each entry. */
	for (i = 0; i < entry_count; i++) {
		if(fread(&namelen_n, sizeof(namelen_n), 1, fp) != 1) {
			printf("read_cache: error reading namelen.\n");
			printf("read_cache: failed at file position %ld\n", ftell(fp));
			return 1;
		}
		entry.namelen = namelen_n;

		
		printf("read_cache: namelen_n: %u\n", namelen_n);
	        printf("read_cache: entry.namelen: %u\n", entry.namelen);

		entry.name = (char *)malloc(entry.namelen + 1);
		if (entry.name == NULL) {
			printf("read_cache: Error allocating memory for entry.name.\n");
			return 1;
		}

		if (fread(entry.name, entry.namelen, 1, fp) != 1) {
			printf("read_cache: error reading name.\n");
			printf("read_cache: failed at file position %ld\n", ftell(fp));
			free(entry.name);
			return 1;
		}
		entry.name[entry.namelen] = '\0';

		if (fread(&mode_n, sizeof(mode_n), 1, fp) != 1) {
			printf("read_cache: error reading file mode.\n");
			free(entry.name);
			return 1;
		}
		entry.mode = ntohl(mode_n);

		if (fread(&size_n, sizeof(size_n), 1, fp) != 1) {
			printf("read_cache: error reading file size.\n");
			free(entry.name);
			return 1;
		}
		entry.size = ntohl(size_n);

		if (fread(&entry.sha, SHA_DIGEST_LENGTH, 1, fp) != 1) {
			printf("read_cache: error reading file SHA-1 hash.\n");
			free(entry.name);
			return 1;
		}

		/* Calculate the size of the entry, including the metadata and the data. */
		entry_size = sizeof(unsigned short) + entry.namelen + sizeof(unsigned int) * 2 + SHA_DIGEST_LENGTH + entry.size;

		/* Skip to the next entry */
		if (fseek(fp, entry_size, SEEK_CUR) != 0) {
			printf("Error seeking to next entry\n");
			return 1;
		}

		printf("read_cache: entry.namelen: %d\n", entry.namelen);
		printf("read_cache: entry.name: %s\n", entry.name);
		printf("read_cache: entry.mode: %o\n", entry.mode);
		printf("read_cache: entry.size: %u bytes\n", entry.size);
	}

	fclose(fp);

	return 0;
}
