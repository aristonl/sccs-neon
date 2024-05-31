#ifndef NEON_H
#define NEON_H

#include <time.h>

typedef struct {
	int depot_id;
	char* name;
	char* desc;
	char* owner;
	char* cid; /* commit id */
} depot_table;

typedef struct {
	int proj_id;
	int depot_id;
	char* name;
	char* desc;
	char* cid; /* commit id */
} proj_table;

typedef struct {
	int proj_id;
	int file_id;
	int rev;
	char* name;
	char* path;
	int size;
} file_table;

typedef struct {
	int rev_id;
	int proj_id;
	char* author;
	time_t timestamp;
	char* mesg;
	char* hash;
} rev_table;

typedef struct {
	int file_rev_id;
	int file_id;
	int rev_id;
} file_rev_table;

#endif /* NEON_H */
