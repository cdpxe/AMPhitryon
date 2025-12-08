#!/bin/bash
export LANG=C

rm -f out.file.sum
rm -f log
echo > new.dict
echo > new.dictb
counter=1

if [ "$1" = "" ]; then
      CHUNKSIZE=6
else
      CHUNKSIZE=$1
fi

while read -r line; do
   echo $line > message
   ./amphi $CHUNKSIZE new.dict message compr.out incr
   echo -n "chunk ${counter}: "
   ls -l message compr.out | awk 'BEGIN{ prev=0; }
            {
               if (prev==0){prev=$5;}
               else {now=$5-prev ; print "saved "now" bytes (compressed="prev" bytes, original="$5" bytes in "$9"). Factor: "prev/$5"";}
            }' | tee -a log
   cat amphi.stats.log | awk -F\; '{print "   pointers: "$1",  plain chunks: "$2",  ratio: " ($2!=0 ? $1/$2 : $1)}'
   counter=$((counter+1))
done < \#ubuntu.txt
echo
cat log | awk 'BEGIN{sum=0;lines=0;} {sum = sum + $2 ; lines++;} END{print "**saved "sum/lines" bytes on average, i.e., "sum " bytes within " lines" lines";}'
