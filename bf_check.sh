#!/bin/bash -e
# bf_check (brute force amphi checker). Written by S. Wendzel, 2025/2026.
# Tested using macOS and Debían.
# This script simply runs amphi + deamphi many, many times and checks whether
# the hash value of the decompressed result is equal to the original value.

# configuration parameters
CHUNK_SIZE_LIST="4 5 6 7 8 9 10 12 15 20 25 30 35 40 45 50"
aDICT=empty.dict_a # for incr
bDICT=empty.dict_b # for incr

# static mode
NUM_OF_BINARIES=10
echo '--------- (1) TESTING STATIC MODE ---------'
cp /usr/bin/gcc ./ # we will use a binary as a dictionary
for CHUNK_SIZE in $CHUNK_SIZE_LIST ; do
    echo "Chunk size: ${CHUNK_SIZE}:"
    for MSG_FILE in `ls /usr/bin | head -n $NUM_OF_BINARIES`; do
        echo -n "  ${MSG_FILE} ..."
        # amphi chunk-size  dict-file    msg-file  compr-file ["incr"] [verbose]
        ./amphi $CHUNK_SIZE ./gcc /usr/bin/${MSG_FILE} compr.out
        # deamphi chunk-size dict-file compr-file out-file ["incr"] [verbose]
        ./deamphi $CHUNK_SIZE /usr/bin/gcc compr.out out.file
        md5_orig=`md5sum /usr/bin/$MSG_FILE | awk '{print $1}'`
        md5_amph=`md5sum out.file | awk '{print $1}'`
        if [ "$md5_orig" = "$md5_amph" ]; then
            echo "ok"; # all fine
        else
            echo "ERROR on CHUNK_SIZE=$CHUNK_SIZE, MSG_FILE=$MSG_FILE"
        fi
    done
done

# incr mode
NUM_OF_BINARIES=50
echo; echo; echo
echo '--------- (2) TESTING INCREMENTAL MODE ---------'
for CHUNK_SIZE in $CHUNK_SIZE_LIST ; do
    echo > $aDICT; echo > $bDICT; # reset the dictionary files of CS and CR
    echo "Chunk size: ${CHUNK_SIZE}:"
    for MSG_FILE in `ls /usr/bin | head  -n $NUM_OF_BINARIES`; do
        echo -n "  ${MSG_FILE} ..."
        # amphi chunk-size  dict-file    msg-file  compr-file ["incr"] [verbose]
        ./amphi $CHUNK_SIZE $aDICT /usr/bin/${MSG_FILE} compr.out incr
        # deamphi chunk-size dict-file compr-file out-file ["incr"] [verbose]
        ./deamphi $CHUNK_SIZE $bDICT compr.out out.file incr
        md5_orig=`md5sum /usr/bin/$MSG_FILE | awk '{print $1}'`
        md5_amph=`md5sum out.file | awk '{print $1}'`
        if [ "$md5_orig" = "$md5_amph" ]; then
            echo "ok"; # all fine
        else
            echo "ERROR on CHUNK_SIZE=$CHUNK_SIZE, MSG_FILE=$MSG_FILE"
        fi
    done
done

