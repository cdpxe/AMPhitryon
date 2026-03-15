/* AMPhitryon header file -- amphi.h, see `LICENSE' file for license details
 * (C) Steffen Wendzel, www.wendzel.de, 2026  */
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#define MAX_CHUNK_SIZE 56 /* this won't be reached */
#define MAX_DICT_LEN 8388607 /* = 2^23 bits -1 */
#define MAX_PLAIN_BLOCKS 126

int get_file_len(char *);
