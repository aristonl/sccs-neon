/*
 * NEON - A simple project revision tracker
 *
 * Copyright (c) 2023 Ariston Lorenzo.
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

#ifndef lint
static const char copyright[] =
"@(#) Copyright (c) 2021, 2022, 2023\n\
	Ariston Lorenzo. All rights reserved.";
#endif /* not lint */

#include "sha1.h"

#include <stdio.h>

#define SHA_DIGEST_LENGTH 20

int
sha_hash_file (char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (!file) {
		printf("Error opening file %s\n", filename);
		return 1;
	}

	SHA_CTX ctx;
	SHA1_Init(&ctx);

	unsigned char buffer[1024 * 1024];
	int bytesRead = 0;

	while ((bytesRead = fread(buffer, 1, 1024 * 1024, file))) {
		SHA1_Update(&ctx, buffer, bytesRead);
	}

	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1_Final(hash, &ctx);

	int i;
	for (i = 0; i < SHA_DIGEST_LENGTH; i++) {
		printf("%02x", hash[i]);
	}

	printf("  %s\n", filename);

	fclose(file);

	return 0;
}

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s filename\n", argv[0]);
		return 1;
	} else {
		sha_hash_file(argv[1]);
	}
}
