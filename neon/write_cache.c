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
#include "sha1.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef lint
static const char copyright[] =
"@(#) Copyright (c) 2021, 2022, 2023, 2024\n\
	Ariston Lorenzo. All rights reserved.\n";
#endif /* not lint */

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

static unsigned int update_num_of_entries(unsigned int num) {
	FILE *fp;

	struct cache_header header;

	fp = fopen(".projdir/cache", "r+");

	fread(&header, sizeof(header), 1, fp);

	header.ent = htonl(num);

	fseek(fp, 0, SEEK_SET);
	fwrite(&header, sizeof(header), 1, fp);

	fclose(fp);

	return 0;
}

#ifndef SHA_DIGEST_LENGTH
#define SHA_DIGEST_LENGTH 20
#endif

int write_cache(char *path) {
	/* Prepare the new_entry_count before hand */
	unsigned int entry_count = get_number_of_entries();
	unsigned int new_entry_count = entry_count + 1;

	FILE *fp;
	FILE *fp_path;
	struct stat st;

	struct cache_entry entry;

	unsigned char buffer[1024 * 1024];
	int bytesRead = 0;

	int i;

	unsigned short namelen_n = htonl(entry.namelen);
	unsigned int mode_n = htonl(entry.mode);
	unsigned int size_n = htonl(entry.size);
	unsigned int entry_size = sizeof(namelen_n) + entry.namelen +
		sizeof(mode_n) + sizeof(size_n) + SHA_DIGEST_LENGTH;
	char *buf = (char *)malloc(entry_size);
	char *p = buf;

	SHA_CTX ctx;

	printf("write_cache: entry_count: %u", entry_count);
	printf("write_cache: new_entry_count: %u", new_entry_count);

	printf("write_cache: %s\n", path);
	verify_header();

	if (access(path, F_OK) == -1) {
		printf("Error: File does not exist.\n");
		return 1;
	}

	if (stat(path, &st) == -1) {
		printf("Error: Could not stat file.\n");
		return 1;
	}

	entry.namelen = strlen(path);
	entry.name = path;
	entry.mode = st.st_mode & 0777;
	entry.size = st.st_size;

	fp_path = fopen(path, "r");
	if (fp_path == NULL) {
		printf("Error opening file");
		return 1;
	}

	/* Create SHA1 Context*/
	SHA1_Init(&ctx);

	/* Create SHA1 Hash */
	while ((bytesRead = fread(buffer, 1, 1024 * 1024, fp_path))) {
		SHA1_Update(&ctx, buffer, bytesRead);
	}

	/* Finalize the context and save the hash to entry.sha */
	SHA1_Final(entry.sha, &ctx);

	printf("write_cache: entry.namelen: %d\n", entry.namelen);
	printf("write_cache: entry.name: %s\n", entry.name);
	printf("write_cache: entry.mode: %o\n", entry.mode);
	printf("write_cache: entry.size: %u bytes\n", entry.size);

	printf("write_cache: SHA1 hash: ");
	for (i = 0; i < SHA_DIGEST_LENGTH; i++) {
		printf("%02x", entry.sha[i]);
	}

	printf("\n");

	fclose(fp_path);

	memcpy(p, &namelen_n, sizeof(namelen_n));
	p += sizeof(namelen_n);
	memcpy(p, entry.name, entry.namelen);
	p += entry.namelen;
	memcpy(p, &mode_n, sizeof(mode_n));
	p += sizeof(mode_n);
	memcpy(p, &size_n, sizeof(size_n));
	p += sizeof(size_n);
	memcpy(p, &entry.sha, SHA_DIGEST_LENGTH);

	fp = fopen(".projdir/cache", "a");
	if (fp == NULL) {
		printf("Error opening cache file");
		return 1; 
	}

	fwrite(buf, entry_size, 1, fp);

	update_num_of_entries(new_entry_count);

	fclose(fp);

	printf("write_cache: %s written to cache\n", path);
	printf("write_cache: Number of entries: %u\n", new_entry_count);

	return 0;
}

int version(void) {
	printf("NEON version %s\n\n", NEON_VERSION);
	printf("Copyright (c) 2021, 2022, 2023, 2024 Ariston Lorenzo. All rights resevred.\n");
	return 0;
}


int main(int argc, char *argv[]) {
	static char usage[] = "Usage: neon-write-cache [-dhv] <file>\n";
	int ch;

	while ((ch = getopt(argc, argv, "dhv")) != -1) {
		switch (ch) {
		case 'd':
#define DEBUG
			break;
		case 'h':
			printf("%s", usage);
			break;
		case 'v':
			version();
			break;
		default:
		case '?':
			printf("%s", usage);
			break;
		}
	}

	if (argv[1] == NULL) {
		printf("%s", usage);
		return 1;
	}

	return 0;
}
