GDB=-g -ggdb
CC=cc
CFLAGS=-I. -Wall $(GDB) -DDEBUG

all : amphi deamphi

amphi : amphi.c lib.c amphi.h
	$(CC) $(CFLAGS) -o amphi amphi.c lib.c

deamphi : deamphi.c lib.c amphi.h
	$(CC) $(CFLAGS) -o deamphi deamphi.c lib.c

clean :
	rm -f amphi deamphi

count :
	wc -l amphi.c amphi.h deamphi.c lib.c

