#include "cache.h"

int verify_header(void) {
    FILE *fp;

    struct cache_header header;

    fp = fopen(".projdir/cache", "r");
    if (fp == NULL) {
        printf("Error opening cache file");
        return 1;
    }

    fread(&header, sizeof(header), 1, fp);

    /*
     * Here's where the actual checking starts.
     *
     * Notice that we htonl() all the hex values...
     * For example, if the signature wasn't htonl'ed, it'd be HSAC instead
     * of CASH.
     */
    if (header.sig != htonl(0x43415348)) {
        printf("Error: Invalid cache signature. Cache file may be corrupt.\n");
        printf("Expected: 0x43415348, Actual: %x\n", header.sig);
        exit(1);
    }
    printf("Cache signature verified: 0x43415348\n");

    if (header.ver == htonl(0x00000001)) {
        printf("Cache version: 1\n");
    } else {
        printf("Error: Invalid cache version. Cache file may be corrupt.\n");
        printf("Expected: 1, Actual: %d\n", header.ver);
        exit(1);
    }

    printf("Number of entries: %d\n", header.ent);

    fclose(fp);

    return 0;
}