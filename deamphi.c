 /* AMPhitryon's decompressor, see `LICENSE' file for license details
 * (C) Steffen Wendzel, www.wendzel.de, 2026 */
#include "amphi.h"

int main(int argc, char *argv[])
{
	char *comprfile = NULL;
	char *dictfile = NULL;
	int CHUNK_SIZE = 10;
	u_int8_t magic_num = 0;
	int i = 0;
	int dict_open_mode = 0;
	int incr = 0;
	int fp_compr, fp_dict, fp_out;
	long dictfile_len, comprfile_len;
	int verbose=0;
	unsigned int read_bytes_from_dict = 0, read_bytes_from_compr = 0;
	char *dictbuf = NULL, *comprbuf = NULL, *outfile = NULL;
	u_int32_t *offset;
	u_int16_t low16_offset; /* least 16 bits of our offset */
	int bytes_written = 0;
	
	if (argc < 5) {
		fprintf(stderr, "usage: ./deamphi chunk-size dict-file compr-file out-file [\"incr\"] [verbose]\n"
				" - The chunk-size must be >=4 bytes as the pointer is 4 bytes.\n"
				" - dict-file is the file the tool uses as its 'dictionary'.\n"
				" - compr-file is the compressed file that will be created.\n"
				" - out-file is the filename where the de-compressed data will be placed (truncated!).\n"
				" - \"incr\" turns on incremental mode (alters the dict-file and can only be used if\n"
				"   'amphi' also uses the incr mode)!)\n"
				" - verbose: '1' turns on verbose mode (default: '0'), '2' is very verbose.\n");
		exit(1);
	}
	
	CHUNK_SIZE = atoi(argv[1]);
	assert(CHUNK_SIZE < MAX_CHUNK_SIZE && CHUNK_SIZE >= 4);
	
	dictfile = argv[2];
	comprfile = argv[3];
	outfile = argv[4];
	
	if (argc >= 6) {
		if (strstr(argv[5], "incr") != NULL)
			incr = 1;
	}
	if (argc >= 7)
		verbose = atoi(argv[6]);
	assert(verbose >= 0 && verbose <= 3);
	
	if (verbose)
		printf("dictfile=%s\ncomprfile=%s\noutfile=%s\nincr=%i\nverbosity=%i\n", dictfile, comprfile, outfile, incr, verbose);

	
	if (!(offset = malloc(sizeof(u_int32_t)))) {
		perror("malloc(offset)"); exit(1);
	}
		
	/* *** dict file handling */
	if (incr)
		dict_open_mode = O_RDWR;
	else
		dict_open_mode = O_RDONLY;
	
	if ((fp_dict = open(dictfile, dict_open_mode)) == -1) {
		perror("open(dictfile)"); exit(1);
	}	
	dictfile_len = get_file_len(dictfile);
	if (verbose>=2) printf("length=%li bytes\n", dictfile_len);
	if (!(dictbuf = (void *) malloc(dictfile_len + 1))) {
		perror("malloc"); exit(1);
	}
	bzero(dictbuf, dictfile_len + 1);
	if (!(read_bytes_from_dict = read(fp_dict, dictbuf, dictfile_len))) {
		perror("read(fp_dict)"); exit(1);
	} else {
		if (verbose>=2) printf("successfully read %d bytes from dictfile.\n", read_bytes_from_dict);
	}
	
	/* *** compr file handling *** */
	if ((fp_compr = open(comprfile, O_RDONLY)) == -1) {
		perror("open(comprfile)"); exit(1);
	}
	comprfile_len = get_file_len(comprfile);
	if (verbose >= 2) printf("length=%li bytes\n", comprfile_len);
	if (!(comprbuf = (void *) malloc(comprfile_len + 1))) {
		perror("malloc"); exit(1);
	}
	bzero(comprbuf, comprfile_len + 1);
	if (!(read_bytes_from_compr = read(fp_compr, comprbuf, comprfile_len))) {
		perror("read(fp_compr)"); exit(1);
	} else {
		if (verbose>=2) printf("successfully read %d bytes from comprfile.\n", read_bytes_from_compr);
	}
	
	/* *** outfile *** */
	if ((fp_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
		perror("open(outfile)"); exit(1);
	}
	
	for (i = 0; i < read_bytes_from_compr; i++) {
		magic_num = (u_int8_t) *(comprbuf+i);
		if (verbose >= 2) printf("   magic-num: %i\n", magic_num);
		if (magic_num < 0x80) {
			if (magic_num > 0) {
				/* we have at least one chunk: Plain-byte Area */
				write(fp_out, (char *)(comprbuf + i + 1), magic_num * CHUNK_SIZE);
				
				/* In incremental mode, we need to add the plain data to the dict-file
				 * and also to our dictbuf and adjust the length variable dictfile_len.
				 * Note: the dictionary could grow larger than MAX_DICT_LEN but takes no effect as
				 * amphi does not point to it. For this reason, we do not extent the dictionary
				 * in such a case (to prevent unnecessarily large dictionary files).
				 * We might use some kind of circular buffer in future versions to improve the
				 * performance on long-lasting flows in which the content changes over time.
				 */
				if (incr && ( dictfile_len + (magic_num * CHUNK_SIZE) ) < MAX_DICT_LEN) {
					int bytes = 0;
					if (verbose >= 2) {
						printf ("extending dictionary file\n");
						printf(" dict-extens. string=%4s[...])\n", (char *)(comprbuf + i + 1));
					}
					lseek(fp_dict, 0, SEEK_END);
					if (!(bytes = write(fp_dict, (char *)(comprbuf + i + 1), magic_num * CHUNK_SIZE))) {
						perror("error during write to dict file"); exit(1);
					}
					if (verbose >= 2) { printf("wrote %i bytes to dictfile\n", bytes); }
										
					/* (2) realloc() memcopy of dict-file and extend it with the same data */
					dictbuf = (void *) realloc(dictbuf, dictfile_len + (magic_num * CHUNK_SIZE) + 1 );
					if (dictbuf == NULL) { perror("realloc(dictbuf)"); exit(1); }
					memcpy(dictbuf + dictfile_len, (char *)(comprbuf + i + 1), magic_num * CHUNK_SIZE);
					dictfile_len += magic_num * CHUNK_SIZE;
				}
				
				/* don't forget to increment i */
				i += magic_num * CHUNK_SIZE;
			} else {
				int num_bytes = 0;
				/* magic byte is 0, so we just have the remaining <CHUNK_SIZE bytes here */
				num_bytes = write(fp_out, (char *)(comprbuf + i + 1), read_bytes_from_compr-i-1); /* writes the remaining bytes */
				if (verbose >= 2) printf("       wrote trailing %i bytes\n", num_bytes);
				i = read_bytes_from_compr; /* end the loop, exit the program */
			}
		} else { /* magic num is >=0x80, i.e., a pointer */
			*offset = (u_int32_t) (magic_num & 0x7f); /* use the 7 LSB of the magic number byte */
			*offset <<= 16; /* shift these bits by 2 bytes so that we can fill the least sign. two bytes with offset data from file */
			memcpy(&low16_offset, comprbuf+i+1, 2);
			*offset = *offset + low16_offset;
			if (verbose > 2) printf("Compiled offset: 0x%x\n", *offset);

			if (*offset >= dictfile_len) {
				printf("offset %u > dictfile_len %ld at i=%d. This should not happen!\n", *offset, dictfile_len, i);
				exit(1);
			}
			if (verbose) {
				printf("      offset=%u (0x%x) (raw=%d)\n", *offset, *offset, (int) *(comprbuf+i+1));
				printf("       accessing addr %p + %x = %p vs %x\n", comprbuf, *offset,
						(comprbuf+*offset), (int)*(comprbuf+i+1));
			}
			if (*offset < 0 && *(offset)+3 <= dictfile_len) {
				fprintf(stderr,
					"assertion failed (invalid offset value, offset=%i, dictfile_len=%ld)\n",
					*offset, dictfile_len);
				exit(1);
			}
			if ((bytes_written = write(fp_out, (dictbuf+*offset), CHUNK_SIZE)) <= 0) {
				perror("write"); fprintf(stderr, "writing error during write(%i, %i)\n", fp_out, CHUNK_SIZE);
			}
			if (verbose >= 2) {
				printf("        wrote %i bytes (%x,%x,%x,%x[...])\n", bytes_written,
					(u_int8_t)*(dictbuf+*offset), (u_int8_t)*(dictbuf+*offset+1), (u_int8_t)*(dictbuf+*offset+2), (u_int8_t)*(dictbuf+*offset+3) );
			}
			i += sizeof(u_int16_t);
		}
	}
	
	close(fp_dict);
	close(fp_compr);
	close(fp_out);
	return 0;
}

