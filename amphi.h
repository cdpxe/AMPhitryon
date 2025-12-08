/* AMPhitryon header file -- amphi.h, see `LICENSE' file for license details
 * (C) Steffen Wendzel, www.wendzel.de, 2024, 2025  */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#define _GNU_SOURCE
#include <string.h>
#include <strings.h>
#include <assert.h>

#define MAX_CHUNK_SIZE 56 /* this won't be reached */

#define MAX_PLAIN_BLOCKS 126

int get_filelen(char *);
