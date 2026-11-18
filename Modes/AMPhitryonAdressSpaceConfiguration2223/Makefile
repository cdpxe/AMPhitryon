GDB=-g -ggdb
CC=cc
CFLAGS=-I. -Wall $(GDB) -DDEBUG

all : amphi deamphi

amphi : amphi.c lib.c amphi.h
	$(CC) $(CFLAGS) -o variableAmphi amphi.c lib.c -lc

deamphi : deamphi.c lib.c amphi.h
	$(CC) $(CFLAGS) -o variableDeamphi deamphi.c lib.c -lc

clean :
	rm -f variableAmphi variableDeamphi

count :
	wc -l amphi.c amphi.h deamphi.c lib.c

