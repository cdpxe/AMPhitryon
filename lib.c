/* AMPhitryon's functions for both, its compressor and decompressor.
 * See `LICENSE' file for license details
 * (C) Steffen Wendzel, www.wendzel.de, 2024, 2025  */
#include "amphi.h"

int get_file_len(char *filename)
{
	int len;
	FILE *fp;
	if ((fp = fopen(filename, "r")) == NULL) {
		perror(filename);
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fclose(fp);
	return len;
}

