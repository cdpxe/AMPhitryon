 /* AMPhitryon's compressor, see `LICENSE' file for license details
 * (C) Steffen Wendzel, www.wendzel.de, 2024, 2025.
 * Time measurement code taken from the CCEAP tool, (C) S. Wendzel, 2019 */
#include "amphi.h"

//#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <netinet/tcp.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

#ifdef __MACH__
 #include <mach/clock.h>
 #include <mach/mach.h>
#endif


#define WRITE_PLAIN 		if (verbose) printf("writing plain blocks (%i).\n", write_plain); \
				/* first write the number of plain blocks here */ \
				if (!(write(fp_compr, &write_plain, 1 /* 1 byte */))) { \
					perror("write"); exit(1); \
				} \
				/* now the plain blocks themselves */ \
				if (!(write(fp_compr, plain_buf, write_plain * CHUNK_SIZE))) { \
					perror("write to compr file"); exit(1); \
				} \
				/* in case of incr mode: only write additional content to dictfile if there */ \
				/* is still space left! */ \
				if (incr && MAX_DICT_LEN > (dictfile_len + (write_plain * CHUNK_SIZE))) { \
					int bytes = 0; \
					/* (1) write chunks to dict-file (extend the file */ \
					if (verbose >= 2) { printf ("extending dictionary file.\n"); } \
					lseek(fp_dict, 0, SEEK_END); \
					if (!(bytes = write(fp_dict, plain_buf, write_plain * CHUNK_SIZE))) { \
						perror("error during write to dict file"); exit(1); \
					} \
					if (verbose >= 2) { printf("wrote %i bytes to dictfile\n", bytes); } \
					/* (2) realloc() memcopy of dict-file and extend it with the same data */ \
					dictbuf = (void *) realloc(dictbuf, dictfile_len + (write_plain * CHUNK_SIZE) + 1 ); \
					if (dictbuf == NULL) { perror("realloc(dictbuf)"); exit(1); } \
					memcpy(dictbuf + dictfile_len, plain_buf, write_plain * CHUNK_SIZE); \
					dictfile_len += write_plain * CHUNK_SIZE; \
					/* Don't forget to update the amp_ptr (we cannot reach a pointer > MAX_DICT_LEN due to the check above */ \
					amp_ptr = (void *) memmem(dictbuf, (dictfile_len < MAX_DICT_LEN ? dictfile_len : MAX_DICT_LEN), buf, CHUNK_SIZE); \
				} else { if (incr && verbose) fprintf(stderr, "reached max. dictionary size (max=%d, len is now=%ld). write_plain=%d, chunk size=%d!\n", MAX_DICT_LEN, dictfile_len, write_plain, CHUNK_SIZE); } \
				write_plain = 0; \
				written_magic_bytes_for_blocks++; \
				bzero(plain_buf, sizeof(plain_buf));


/* just report some stats */
#define STATS_LOGFILE		"amphi.stats.log"
void report_magic_byte(int written_magic_bytes_for_ptrs, int written_magic_bytes_for_blocks, int verbose)
{
	int fp_log;
	if ((fp_log = open(STATS_LOGFILE, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
		perror("open("STATS_LOGFILE")"); exit(1);
	}
	dprintf(fp_log, "%i;%i;", written_magic_bytes_for_ptrs, written_magic_bytes_for_blocks);
	close(fp_log);
	if (verbose)
		printf("---\nSummary:   pointers: %i; plain blocks: %i\n", written_magic_bytes_for_ptrs, written_magic_bytes_for_blocks);
}

void print_time_diff(void) /* taken from CCEAP */
{
	long ms;
	time_t s;
	struct timespec spec_now;
	static struct timespec spec_last;
	static int first_call = 1;
#ifdef __MACH__ /* code from Stackoverflow.com (Mac OS lacks clock_gettime()) */
	clock_serv_t cclock;
	mach_timespec_t mts;

	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	spec_now.tv_sec = mts.tv_sec;
	spec_now.tv_nsec = mts.tv_nsec;
#else
	clock_gettime(CLOCK_REALTIME, &spec_now);
#endif
	
	if (first_call) {
		first_call = 0;
	} else {
		s  = spec_now.tv_sec - spec_last.tv_sec;
		ms = (spec_now.tv_nsec - spec_last.tv_nsec) / 1.0e6;	
		if (ms < 0) {
			ms = 1000 + ms;
			s -= 1;
		}

		printf("Compression time: %"PRIdMAX".%03ld\n", (intmax_t)s, ms);
	}
	bcopy(&spec_now, &spec_last, sizeof(struct timespec));
}

int main(int argc, char *argv[])
{
	char *dictfile = NULL;
	char *msgfile = NULL;
	char *comprfile = NULL;
	int incr = 0;
	int fp_dict, fp_msg, fp_compr;
	long dictfile_len;
	int CHUNK_SIZE;
	char buf[MAX_CHUNK_SIZE+1];
	void *dictbuf = NULL;
	void *amp_ptr; /* can reach values of up to 2^32 but we can only store 2^23 (7+16). For this reason, we will later shift bits. */
	u_int32_t offset;
	u_int16_t low16_offset; /* least 16 bits of our offset */
	int verbose = 0;
	char plain_buf[MAX_CHUNK_SIZE*0xff];
	int read_bytes_from_dict = 0;
	u_int8_t magic_pointer_value = 0x80, magic_byte_last_chunk = 0;
	u_int8_t write_plain = 0; /* number of plain blocks to write */
	int byte_count = 0;
	int bytes_written = 0;
	/* some stats for further analysis */
	int written_magic_bytes_for_ptrs = 0;
	int written_magic_bytes_for_blocks = 0;
	
	if (argc < 5) {
		fprintf(stderr, "usage: ./amphi chunk-size dict-file msg-file compr-file [\"incr\"] [verbose]\n"
				" - The chunk-size must be >=4 bytes as the pointer is 4 bytes.\n"
				" - dict-file is the file the tool uses as its 'dictionary'.\n"
				" - msg-file is the file that must be compressed.\n"
				" - compr-file is the compressed file that will be created.\n"
				" - \"incr\" turns on INCREMENTAL mode (non-matched chunks will be\n"
				"    added to the dict-file!). Can only be used if deamphi also uses the \"incr\"\n"
				"    option and the same starting dict-file!\n"
				" - verbose: '1' turns on verbose mode (default: '0'), '2' is very verbose.\n");
		exit(1);
	}
	
	CHUNK_SIZE = atoi(argv[1]);
	assert(CHUNK_SIZE < MAX_CHUNK_SIZE && CHUNK_SIZE >= 4);
	
	dictfile = argv[2];
	msgfile = argv[3];
	comprfile = argv[4];
	if (argc >= 6) {
		if (strstr(argv[5], "incr") != NULL)
			incr = 1;
	}
	if (argc >= 7)
		verbose = atoi(argv[6]);
	assert(verbose >= 0 || verbose <= 3);
	
	if (verbose)
		printf("dictfile=%s\nmsgfile=%s\ncomprfile=%s\nincr=%i\nverbosity=%i\n", dictfile, msgfile, comprfile, incr, verbose);
	
	if ((fp_dict = open(dictfile, O_RDWR)) == -1) {
		perror("open(dictfile)"); exit(1);
	}
	
	if ((fp_msg = open(msgfile, O_RDONLY)) == -1) {
		perror("open(msgfile)"); exit(1);
	}
	
	if ((fp_compr = open(comprfile, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
		perror("open(comprfile)"); exit(1);
	}

	if (verbose>=2)
		printf("offset type size is %lu bytes, chunk size is %d bytes\n",
			sizeof(offset), CHUNK_SIZE);
	/* Our approach only makes sense if the history covert channel pointer (offset)
	 * is smaller than the size of a chunk we refer to. However, we accept equality,
	 * too, for testing. */
	assert(sizeof(offset) <= CHUNK_SIZE);

	if (verbose>=2) printf("reading %s... ", dictfile);
	
	dictfile_len = get_file_len(dictfile);
	if (verbose>=2) printf("length=%li bytes\n", dictfile_len);
	if (!(dictbuf = (void *) malloc(dictfile_len + 1))) {
		perror("malloc"); exit(1);
	}
	bzero(dictbuf, dictfile_len + 1);
	read_bytes_from_dict = read(fp_dict, dictbuf, dictfile_len);
	if (read_bytes_from_dict == -1 || read_bytes_from_dict == 0) {
		perror("read(fp_dict)"); exit(1);
	} else {
		if (verbose>=2) printf("successfully read %d bytes from dictfile.\n", read_bytes_from_dict);
	}
	
	print_time_diff(); /* start measuring computational time, since preparation is done now */
	bzero(plain_buf, sizeof(plain_buf));
	bzero(buf, sizeof(buf));
	while ((byte_count = read(fp_msg, buf, CHUNK_SIZE))) {
		if (verbose >= 2)
			printf("read: 0x%x (string=%c%c%c%c[...])\n", (unsigned int)*buf, buf[0], buf[1], buf[2], buf[3]);
		/* try to get an amp_ptr match (chk if we can find buf in the dictfile), but make sure the pointer fits in the
		 * max. bitlength of the 23 pointer bits. */
		amp_ptr = (void *) memmem(dictbuf, (dictfile_len < MAX_DICT_LEN ? dictfile_len : MAX_DICT_LEN), buf, CHUNK_SIZE);
		if (amp_ptr && byte_count == CHUNK_SIZE /* must be chunk size, otherwise we accidentally matched trailing 0 bits */) {
		
			/* before writing this POINTER, check if there's PLAIN stuff to be written first */
			if (write_plain) {
				WRITE_PLAIN
			}
			
			offset = (unsigned int) (amp_ptr - dictbuf);
			if (offset >= dictfile_len) {
				fprintf(stderr, "offset > dictfile_len. This should not happen!\n");
				exit(1);
			}
			if (verbose >= 2) {
				printf("   => Offset is %i (0x%x)\n", offset, offset);
				printf("\t{%p - %p = %x}\n", amp_ptr, dictbuf, offset);
			}
			
			/* start with a "1000 0000" octet (x80) so that we know that a pointer follows */
			magic_pointer_value = 0x80; /* MSB is set to 1, all other bits are set to 0 */
			/* Now, we need to embed the first bits of the pointer value into the 7 LSB of that byte */
			magic_pointer_value |= (u_int8_t)((offset & 0x7f0000)>>16);
			/*printf("magic_pointer is now 0x%x\n", magic_pointer_value);*/

			if (!write(fp_compr, &magic_pointer_value, 1 /* 1 byte */)) {
				perror("write"); exit(1);
			}
			written_magic_bytes_for_ptrs++;
			
			/* now write the remaining 23-7 = 16 bits of the pointer */
			low16_offset = (u_int16_t) (offset &= 0xffff); /* keep only the least sign. 16 bits */
			if (!(bytes_written = write(fp_compr, &low16_offset, 2))) {
				perror("write"); exit(1);
			}
			if (bytes_written != 2) {
				fprintf(stderr, "Error: Wrote %i instead of 2 trailing bytes for pointer!\n",
					bytes_written);
				exit(1);
			} else {
				if (verbose >= 2) {
					fprintf(stdout, "Wrote low16_offset = 0x%hx (offset=0x%x)\n", low16_offset, offset);
				}
			}
		} else if (byte_count == CHUNK_SIZE) {
			/* need to check how many plain blocks we need to write,
			 * so we cache this one and start counting. */
			memcpy(plain_buf + (write_plain*CHUNK_SIZE), buf, CHUNK_SIZE);
			write_plain++;
			if (write_plain > MAX_PLAIN_BLOCKS - 1) {
				if (verbose)
					fprintf(stderr, "Overflow of blocks; inserting separate block number!\n");
				WRITE_PLAIN
			}
		} else {
			/* check if we need to do a WRITE_PLAIN of remaining plain blocks first */
			if (write_plain) {
				WRITE_PLAIN
			}
			/* We read less than CHUNK_SIZE bytes and we cannot point to the string,
			 * so we need to write a plain block as a rest, i.e., write 0 as magic value */
			if (!(bytes_written = write(fp_compr, &magic_byte_last_chunk, 1 /* 1 byte */))) {
				perror("write"); exit(1);
			}
			if (verbose >= 2) printf("wrote %i bytes (magic byte)\n", bytes_written);
			/* now write the remaining bytes */
			if (!(bytes_written = write(fp_compr, buf, byte_count))) {
				perror("write"); exit(1);
			}
			if (verbose >= 2) printf("wrote %i out of %i bytes (remaining bytes of file)\n", bytes_written, byte_count);
		}
		
		/* clean buffer */
		bzero(buf, sizeof(buf));
	}
	if (write_plain) {
		if (verbose) printf("writing left-over plain blocks.\n");
		WRITE_PLAIN
	}

	print_time_diff();
	report_magic_byte(written_magic_bytes_for_ptrs, written_magic_bytes_for_blocks, verbose);
	
	close(fp_dict);
	close(fp_msg);
	close(fp_compr);
	return 0;
}

