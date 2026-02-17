#!/bin/bash -e
# runtime_eval.sh: Runtime evaluation for amphi. Written by S. Wendzel, 2025/2026.
# Tested using MacOS.
# This script measures the time it takes to let amphi perform using
# different parameters.

export LANG=C
export CLANG=C
# configuration parameters
CHUNK_SIZE_LIST="4 5 6 7 8 9 10 12 15 20 25 30 35 40 45 50"
DICT_SIZE_LIST="10000 50000 100000 200000 500000 1000000 2000000 4000000 " #5000000 8388606" #max
aDICT=empty.dict_a # for incr
MSG_FILE=msg.file
MESSAGE_SIZES="4 8 10 12 14 18 22 26 30 34 38 42 50"

echo "MessageSize;DictFileSize;ChunkSize;ComputationTime"
for MESSAGE_SIZE in $MESSAGE_SIZES ; do
    #echo "MESSAGE_SIZE=$MESSAGE_SIZE"
    dd if=/dev/zero of=$MSG_FILE count=$MESSAGE_SIZE 2>/dev/null
    for DICTSIZE in $DICT_SIZE_LIST ; do
        #echo "   DICTSIZE=$DICTSIZE"
        dd if=/dev/zero of=$aDICT count=$DICTSIZE 2>/dev/null
        for CHUNK_SIZE in $CHUNK_SIZE_LIST ; do
            #echo "     Chunk size: ${CHUNK_SIZE}:"
            # amphi chunk-size  dict-file    msg-file  compr-file ["incr"] [verbose]
            TIMEDIFF=`./amphi $CHUNK_SIZE $aDICT ${MSG_FILE} compr.out | awk '{print $3}'`
            echo "$MESSAGE_SIZE;$DICTSIZE;$CHUNK_SIZE;$TIMEDIFF"
        done
    done
done
