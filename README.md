# AMPhitryon

This code implements the *AMPhitryon* compression scheme. AMPhitryon is tailored, but not limited, to network packet compression in censorship circumvention/covert channel setups. This code belongs to the following submission:

[Steffen Wendzel](https://scholar.google.de/citations?user=DZqkZ1IAAAAJ&hl=de&oi=ao), [Sebastian Zillien](https://scholar.google.de/citations?user=kdCKry4AAAAJ&hl=de), [Sebastian Zander](https://scholar.google.de/citations?hl=de&user=8csOslsAAAAJ): ***AMPhitryon*: Compression for Small Data Using Local Dictionaries to Improve Covert and Circumvention Channels**, under review.

AMPhitryon is tailored to compress network traffic (or any other kind of data chunks) using an (iterative) dictionary approach. This way, payload sizes below 50 bytes (as small as 4 bytes) can be **compressed in the context of the network flows in which they appear**.

## Why AMPhitryon is Useful

Network traffic fingerprinting (e.g., the detection and identification of circumvention traffic) depends on the amount of traffic that can be observed. While larger packets can be compressed rather easily, **small packets are rarely compressible and thus leave valuable traces for adveraries**. **AMPhitryon addresses this issue and compresses such packets**. For this reason, **fewer traffic data is available for an adversary to conduct passive attacks** such as website fingerprinting (WFP) when censorship circumvention tools or covert channel tools are used.

AMPhitryon is tailored to serve as an extension (extra element of processing packets on the OSI presentation layer) so that it can be used as part of other tools.

## How to use AMPhitryon?

First, we need to compile AMPhitryon. To this end, we run `make`, which builds both components of the tool. `amphi` is used to compress messages, while `deamphi` *de*compresses these messages.

```
$ make
```

Let's suppose that we want to compress chat message (one message at a time to be transferred as a payload). To test this, we download Ubuntu IRC chat logs from a public repository:

```
$ wget https://irclogs.ubuntu.com/2025/11/26/%23ubuntu.txt
```

Next, let's create an empty directory file containing only a newline:

```
$ echo > new.dict
```

Now, we feed every chat message (linewise) to AMPhitryon, which we do in a simple loop:

```
while read -r line; do
   echo $line > message
   ./amphi 5 new.dict message compr.out incr
done < \#ubuntu.txt
```

**What do the parameters mean?** The first parameter ("`5`") is the chunk size that we compress (in bytes). The second parameter (`new.dict`) is the filename in which we store our dictionary. The third parameter tells the tool the filename (`message`) containing the input message that we want to compress. The third parameter (`compr.out`) tells the tool in which file the compressed result must be saved. Finally, "`incr`" activates the incremental mode described in our paper. If the parameter is left, a static dictionary can be used.

While the previous commands seem to work, let's **check if the compression actually works!** Therefore, we compare the size of the compressed result (*compr.out*) with the size of the input (*message* file). To this end, you can execute `run_test.sh` which takes the 116 chat messages to build a very small dictionary.

```
$ ./run_test.sh
chunk 1: saved -2 bytes (compressed=36 bytes, original=34 bytes in message). Factor: 1.05882
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 2: saved 6 bytes (compressed=37 bytes, original=43 bytes in message). Factor: 0.860465
   pointers: 3,  plain chunks: 2,  ratio: 1.5
chunk 3: saved -1 bytes (compressed=31 bytes, original=30 bytes in message). Factor: 1.03333
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 4: saved 10 bytes (compressed=43 bytes, original=53 bytes in message). Factor: 0.811321
   pointers: 4,  plain chunks: 1,  ratio: 4
chunk 5: saved -1 bytes (compressed=31 bytes, original=30 bytes in message). Factor: 1.03333
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 6: saved 3 bytes (compressed=28 bytes, original=31 bytes in message). Factor: 0.903226
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 7: saved -2 bytes (compressed=237 bytes, original=235 bytes in message). Factor: 1.00851
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 8: saved -2 bytes (compressed=31 bytes, original=29 bytes in message). Factor: 1.06897
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 9: saved 7 bytes (compressed=19 bytes, original=26 bytes in message). Factor: 0.730769
   pointers: 3,  plain chunks: 1,  ratio: 3
chunk 10: saved -2 bytes (compressed=37 bytes, original=35 bytes in message). Factor: 1.05714
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 11: saved 13 bytes (compressed=27 bytes, original=40 bytes in message). Factor: 0.675
   pointers: 5,  plain chunks: 1,  ratio: 5
chunk 12: saved 3 bytes (compressed=68 bytes, original=71 bytes in message). Factor: 0.957746
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 13: saved 3 bytes (compressed=26 bytes, original=29 bytes in message). Factor: 0.896552
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 14: saved 3 bytes (compressed=56 bytes, original=59 bytes in message). Factor: 0.949153
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 15: saved 4 bytes (compressed=80 bytes, original=84 bytes in message). Factor: 0.952381
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 16: saved 7 bytes (compressed=18 bytes, original=25 bytes in message). Factor: 0.72
   pointers: 3,  plain chunks: 1,  ratio: 3
chunk 17: saved 3 bytes (compressed=98 bytes, original=101 bytes in message). Factor: 0.970297
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 18: saved 4 bytes (compressed=74 bytes, original=78 bytes in message). Factor: 0.948718
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 19: saved 0 bytes (compressed=31 bytes, original=31 bytes in message). Factor: 1
   pointers: 1,  plain chunks: 2,  ratio: 0.5
chunk 20: saved 4 bytes (compressed=26 bytes, original=30 bytes in message). Factor: 0.866667
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 21: saved 2 bytes (compressed=300 bytes, original=302 bytes in message). Factor: 0.993377
   pointers: 2,  plain chunks: 3,  ratio: 0.666667
chunk 22: saved 8 bytes (compressed=75 bytes, original=83 bytes in message). Factor: 0.903614
   pointers: 4,  plain chunks: 3,  ratio: 1.33333
chunk 23: saved 7 bytes (compressed=56 bytes, original=63 bytes in message). Factor: 0.888889
   pointers: 3,  plain chunks: 1,  ratio: 3
chunk 24: saved 0 bytes (compressed=25 bytes, original=25 bytes in message). Factor: 1
   pointers: 1,  plain chunks: 2,  ratio: 0.5
chunk 25: saved 2 bytes (compressed=90 bytes, original=92 bytes in message). Factor: 0.978261
   pointers: 2,  plain chunks: 3,  ratio: 0.666667
chunk 26: saved 12 bytes (compressed=109 bytes, original=121 bytes in message). Factor: 0.900826
   pointers: 5,  plain chunks: 2,  ratio: 2.5
chunk 27: saved 10 bytes (compressed=146 bytes, original=156 bytes in message). Factor: 0.935897
   pointers: 4,  plain chunks: 2,  ratio: 2
chunk 28: saved 3 bytes (compressed=59 bytes, original=62 bytes in message). Factor: 0.951613
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 29: saved 3 bytes (compressed=46 bytes, original=49 bytes in message). Factor: 0.938776
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 30: saved 7 bytes (compressed=95 bytes, original=102 bytes in message). Factor: 0.931373
   pointers: 3,  plain chunks: 2,  ratio: 1.5
chunk 31: saved 17 bytes (compressed=84 bytes, original=101 bytes in message). Factor: 0.831683
   pointers: 7,  plain chunks: 3,  ratio: 2.33333
chunk 32: saved 10 bytes (compressed=202 bytes, original=212 bytes in message). Factor: 0.95283
   pointers: 5,  plain chunks: 4,  ratio: 1.25
chunk 33: saved 11 bytes (compressed=45 bytes, original=56 bytes in message). Factor: 0.803571
   pointers: 5,  plain chunks: 3,  ratio: 1.66667
chunk 34: saved 9 bytes (compressed=82 bytes, original=91 bytes in message). Factor: 0.901099
   pointers: 4,  plain chunks: 2,  ratio: 2
chunk 35: saved 11 bytes (compressed=106 bytes, original=117 bytes in message). Factor: 0.905983
   pointers: 5,  plain chunks: 3,  ratio: 1.66667
chunk 36: saved 7 bytes (compressed=58 bytes, original=65 bytes in message). Factor: 0.892308
   pointers: 3,  plain chunks: 1,  ratio: 3
chunk 37: saved 5 bytes (compressed=84 bytes, original=89 bytes in message). Factor: 0.94382
   pointers: 3,  plain chunks: 3,  ratio: 1
chunk 38: saved 10 bytes (compressed=87 bytes, original=97 bytes in message). Factor: 0.896907
   pointers: 5,  plain chunks: 4,  ratio: 1.25
chunk 39: saved 13 bytes (compressed=97 bytes, original=110 bytes in message). Factor: 0.881818
   pointers: 6,  plain chunks: 4,  ratio: 1.5
chunk 40: saved 7 bytes (compressed=35 bytes, original=42 bytes in message). Factor: 0.833333
   pointers: 3,  plain chunks: 2,  ratio: 1.5
chunk 41: saved -1 bytes (compressed=79 bytes, original=78 bytes in message). Factor: 1.01282
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 42: saved 4 bytes (compressed=21 bytes, original=25 bytes in message). Factor: 0.84
   pointers: 2,  plain chunks: 1,  ratio: 2
chunk 43: saved 5 bytes (compressed=85 bytes, original=90 bytes in message). Factor: 0.944444
   pointers: 3,  plain chunks: 4,  ratio: 0.75
chunk 44: saved 12 bytes (compressed=200 bytes, original=212 bytes in message). Factor: 0.943396
   pointers: 6,  plain chunks: 5,  ratio: 1.2
chunk 45: saved 0 bytes (compressed=20 bytes, original=20 bytes in message). Factor: 1
   pointers: 1,  plain chunks: 2,  ratio: 0.5
chunk 46: saved 5 bytes (compressed=132 bytes, original=137 bytes in message). Factor: 0.963504
   pointers: 3,  plain chunks: 3,  ratio: 1
chunk 47: saved 14 bytes (compressed=196 bytes, original=210 bytes in message). Factor: 0.933333
   pointers: 7,  plain chunks: 7,  ratio: 1
chunk 48: saved 11 bytes (compressed=95 bytes, original=106 bytes in message). Factor: 0.896226
   pointers: 5,  plain chunks: 3,  ratio: 1.66667
chunk 49: saved -2 bytes (compressed=24 bytes, original=22 bytes in message). Factor: 1.09091
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 50: saved 6 bytes (compressed=40 bytes, original=46 bytes in message). Factor: 0.869565
   pointers: 3,  plain chunks: 2,  ratio: 1.5
chunk 51: saved 3 bytes (compressed=28 bytes, original=31 bytes in message). Factor: 0.903226
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 52: saved -2 bytes (compressed=28 bytes, original=26 bytes in message). Factor: 1.07692
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 53: saved 8 bytes (compressed=228 bytes, original=236 bytes in message). Factor: 0.966102
   pointers: 5,  plain chunks: 6,  ratio: 0.833333
chunk 54: saved 5 bytes (compressed=65 bytes, original=70 bytes in message). Factor: 0.928571
   pointers: 3,  plain chunks: 3,  ratio: 1
chunk 55: saved 21 bytes (compressed=298 bytes, original=319 bytes in message). Factor: 0.934169
   pointers: 9,  plain chunks: 5,  ratio: 1.8
chunk 56: saved 15 bytes (compressed=43 bytes, original=58 bytes in message). Factor: 0.741379
   pointers: 6,  plain chunks: 2,  ratio: 3
chunk 57: saved 12 bytes (compressed=99 bytes, original=111 bytes in message). Factor: 0.891892
   pointers: 5,  plain chunks: 2,  ratio: 2.5
chunk 58: saved 4 bytes (compressed=115 bytes, original=119 bytes in message). Factor: 0.966387
   pointers: 3,  plain chunks: 4,  ratio: 0.75
chunk 59: saved 6 bytes (compressed=142 bytes, original=148 bytes in message). Factor: 0.959459
   pointers: 3,  plain chunks: 2,  ratio: 1.5
chunk 60: saved 13 bytes (compressed=241 bytes, original=254 bytes in message). Factor: 0.948819
   pointers: 7,  plain chunks: 7,  ratio: 1
chunk 61: saved -2 bytes (compressed=27 bytes, original=25 bytes in message). Factor: 1.08
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 62: saved -2 bytes (compressed=143 bytes, original=141 bytes in message). Factor: 1.01418
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 63: saved 1 bytes (compressed=35 bytes, original=36 bytes in message). Factor: 0.972222
   pointers: 1,  plain chunks: 2,  ratio: 0.5
chunk 64: saved 3 bytes (compressed=29 bytes, original=32 bytes in message). Factor: 0.90625
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 65: saved 10 bytes (compressed=22 bytes, original=32 bytes in message). Factor: 0.6875
   pointers: 4,  plain chunks: 1,  ratio: 4
chunk 66: saved 10 bytes (compressed=41 bytes, original=51 bytes in message). Factor: 0.803922
   pointers: 4,  plain chunks: 1,  ratio: 4
chunk 67: saved 4 bytes (compressed=23 bytes, original=27 bytes in message). Factor: 0.851852
   pointers: 2,  plain chunks: 1,  ratio: 2
chunk 68: saved 3 bytes (compressed=30 bytes, original=33 bytes in message). Factor: 0.909091
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 69: saved 6 bytes (compressed=39 bytes, original=45 bytes in message). Factor: 0.866667
   pointers: 3,  plain chunks: 2,  ratio: 1.5
chunk 70: saved 16 bytes (compressed=27 bytes, original=43 bytes in message). Factor: 0.627907
   pointers: 6,  plain chunks: 1,  ratio: 6
chunk 71: saved 6 bytes (compressed=37 bytes, original=43 bytes in message). Factor: 0.860465
   pointers: 3,  plain chunks: 2,  ratio: 1.5
chunk 72: saved 10 bytes (compressed=41 bytes, original=51 bytes in message). Factor: 0.803922
   pointers: 4,  plain chunks: 1,  ratio: 4
chunk 73: saved 19 bytes (compressed=34 bytes, original=53 bytes in message). Factor: 0.641509
   pointers: 7,  plain chunks: 1,  ratio: 7
chunk 74: saved 3 bytes (compressed=32 bytes, original=35 bytes in message). Factor: 0.914286
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 75: saved 3 bytes (compressed=28 bytes, original=31 bytes in message). Factor: 0.903226
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 76: saved 7 bytes (compressed=26 bytes, original=33 bytes in message). Factor: 0.787879
   pointers: 3,  plain chunks: 1,  ratio: 3
chunk 77: saved 6 bytes (compressed=33 bytes, original=39 bytes in message). Factor: 0.846154
   pointers: 3,  plain chunks: 2,  ratio: 1.5
chunk 78: saved 6 bytes (compressed=33 bytes, original=39 bytes in message). Factor: 0.846154
   pointers: 3,  plain chunks: 2,  ratio: 1.5
chunk 79: saved 21 bytes (compressed=21 bytes, original=42 bytes in message). Factor: 0.5
   pointers: 7,  plain chunks: 0,  ratio: 7
chunk 80: saved 0 bytes (compressed=37 bytes, original=37 bytes in message). Factor: 1
   pointers: 1,  plain chunks: 2,  ratio: 0.5
chunk 81: saved 7 bytes (compressed=32 bytes, original=39 bytes in message). Factor: 0.820513
   pointers: 3,  plain chunks: 1,  ratio: 3
chunk 82: saved 10 bytes (compressed=43 bytes, original=53 bytes in message). Factor: 0.811321
   pointers: 4,  plain chunks: 1,  ratio: 4
chunk 83: saved 9 bytes (compressed=40 bytes, original=49 bytes in message). Factor: 0.816327
   pointers: 4,  plain chunks: 2,  ratio: 2
chunk 84: saved 3 bytes (compressed=36 bytes, original=39 bytes in message). Factor: 0.923077
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 85: saved 4 bytes (compressed=32 bytes, original=36 bytes in message). Factor: 0.888889
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 86: saved 19 bytes (compressed=30 bytes, original=49 bytes in message). Factor: 0.612245
   pointers: 7,  plain chunks: 1,  ratio: 7
chunk 87: saved -1 bytes (compressed=31 bytes, original=30 bytes in message). Factor: 1.03333
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 88: saved 8 bytes (compressed=15 bytes, original=23 bytes in message). Factor: 0.652174
   pointers: 3,  plain chunks: 0,  ratio: 3
chunk 89: saved 11 bytes (compressed=31 bytes, original=42 bytes in message). Factor: 0.738095
   pointers: 4,  plain chunks: 1,  ratio: 4
chunk 90: saved 5 bytes (compressed=102 bytes, original=107 bytes in message). Factor: 0.953271
   pointers: 3,  plain chunks: 3,  ratio: 1
chunk 91: saved 2 bytes (compressed=22 bytes, original=24 bytes in message). Factor: 0.916667
   pointers: 1,  plain chunks: 1,  ratio: 1
chunk 92: saved 1 bytes (compressed=28 bytes, original=29 bytes in message). Factor: 0.965517
   pointers: 1,  plain chunks: 1,  ratio: 1
chunk 93: saved 26 bytes (compressed=167 bytes, original=193 bytes in message). Factor: 0.865285
   pointers: 11,  plain chunks: 6,  ratio: 1.83333
chunk 94: saved 7 bytes (compressed=73 bytes, original=80 bytes in message). Factor: 0.9125
   pointers: 4,  plain chunks: 4,  ratio: 1
chunk 95: saved 7 bytes (compressed=32 bytes, original=39 bytes in message). Factor: 0.820513
   pointers: 3,  plain chunks: 1,  ratio: 3
chunk 96: saved 7 bytes (compressed=44 bytes, original=51 bytes in message). Factor: 0.862745
   pointers: 3,  plain chunks: 1,  ratio: 3
chunk 97: saved 6 bytes (compressed=31 bytes, original=37 bytes in message). Factor: 0.837838
   pointers: 3,  plain chunks: 2,  ratio: 1.5
chunk 98: saved 12 bytes (compressed=141 bytes, original=153 bytes in message). Factor: 0.921569
   pointers: 6,  plain chunks: 5,  ratio: 1.2
chunk 99: saved 3 bytes (compressed=53 bytes, original=56 bytes in message). Factor: 0.946429
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 100: saved 10 bytes (compressed=32 bytes, original=42 bytes in message). Factor: 0.761905
   pointers: 4,  plain chunks: 2,  ratio: 2
chunk 101: saved 22 bytes (compressed=148 bytes, original=170 bytes in message). Factor: 0.870588
   pointers: 10,  plain chunks: 7,  ratio: 1.42857
chunk 102: saved 3 bytes (compressed=46 bytes, original=49 bytes in message). Factor: 0.938776
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 103: saved 3 bytes (compressed=30 bytes, original=33 bytes in message). Factor: 0.909091
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 104: saved 19 bytes (compressed=30 bytes, original=49 bytes in message). Factor: 0.612245
   pointers: 7,  plain chunks: 1,  ratio: 7
chunk 105: saved 17 bytes (compressed=22 bytes, original=39 bytes in message). Factor: 0.564103
   pointers: 6,  plain chunks: 0,  ratio: 6
chunk 106: saved 13 bytes (compressed=26 bytes, original=39 bytes in message). Factor: 0.666667
   pointers: 5,  plain chunks: 1,  ratio: 5
chunk 107: saved 4 bytes (compressed=175 bytes, original=179 bytes in message). Factor: 0.977654
   pointers: 2,  plain chunks: 1,  ratio: 2
chunk 108: saved 1 bytes (compressed=34 bytes, original=35 bytes in message). Factor: 0.971429
   pointers: 1,  plain chunks: 1,  ratio: 1
chunk 109: saved 13 bytes (compressed=72 bytes, original=85 bytes in message). Factor: 0.847059
   pointers: 6,  plain chunks: 4,  ratio: 1.5
chunk 110: saved -2 bytes (compressed=23 bytes, original=21 bytes in message). Factor: 1.09524
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 111: saved 18 bytes (compressed=18 bytes, original=36 bytes in message). Factor: 0.5
   pointers: 6,  plain chunks: 0,  ratio: 6
chunk 112: saved -2 bytes (compressed=23 bytes, original=21 bytes in message). Factor: 1.09524
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 113: saved -2 bytes (compressed=285 bytes, original=283 bytes in message). Factor: 1.00707
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 114: saved 5 bytes (compressed=86 bytes, original=91 bytes in message). Factor: 0.945055
   pointers: 3,  plain chunks: 3,  ratio: 1
chunk 115: saved 9 bytes (compressed=41 bytes, original=50 bytes in message). Factor: 0.82
   pointers: 4,  plain chunks: 2,  ratio: 2
chunk 116: saved 3 bytes (compressed=41 bytes, original=44 bytes in message). Factor: 0.931818
   pointers: 2,  plain chunks: 2,  ratio: 1

**saved 6.57759 bytes on average, i.e., 763 bytes within 116 lines
```

Although some messages even *gained* a few bytes, on average, slightly more than 6.5 bytes have been saved per chat message. In sum, we saved 763 bytes within 116 separated messages. Usually, the longer the flow, the more bytes are saved (see our paper).

However, a longer exchange of chat messages would build a larger dictionary, leading to a higher fraction of points. The maximum dictionary size is 8 MByte while the current dictionary contains only 6.1 KBytes.

Let's utilize our existing dictionary for another day of chat messages:

```
$ wget https://irclogs.ubuntu.com/2025/11/01/%23ubuntu.txt -O \#ubuntu1101.txt
$ rm -f log
$ counter=1
$ while read -r line; do
   echo $line > message
   ./amphi 6 new.dict message compr.out incr
   echo -n "chunk ${counter}: "
   ls -l message compr.out | awk 'BEGIN{ prev=0; }
            {
               if (prev==0){prev=$5;}
               else {now=$5-prev ; print "saved "now" bytes (compressed="prev" bytes, original="$5" bytes in "$9"). Factor: "prev/$5"";}
            }' | tee -a log
   cat amphi.stats.log | awk -F\; '{print "   pointers: "$1",  plain chunks: "$2",  ratio: " ($2!=0 ? $1/$2 : $1)}'
   counter=$((counter+1))
done < \#ubuntu1101.txt
```

This already yields better results:

````
[...]
chunk 166: saved 21 bytes (compressed=79 bytes, original=100 bytes in message). Factor: 0.79
   pointers: 9,  plain chunks: 5,  ratio: 1.8
chunk 167: saved 4 bytes (compressed=71 bytes, original=75 bytes in message). Factor: 0.946667
   pointers: 3,  plain chunks: 4,  ratio: 0.75
chunk 168: saved 9 bytes (compressed=34 bytes, original=43 bytes in message). Factor: 0.790698
   pointers: 4,  plain chunks: 2,  ratio: 2
chunk 169: saved 13 bytes (compressed=41 bytes, original=54 bytes in message). Factor: 0.759259
   pointers: 5,  plain chunks: 2,  ratio: 2.5
chunk 170: saved 6 bytes (compressed=42 bytes, original=48 bytes in message). Factor: 0.875
   pointers: 3,  plain chunks: 3,  ratio: 1
chunk 171: saved 10 bytes (compressed=28 bytes, original=38 bytes in message). Factor: 0.736842
   pointers: 4,  plain chunks: 1,  ratio: 4
chunk 172: saved 8 bytes (compressed=58 bytes, original=66 bytes in message). Factor: 0.878788
   pointers: 4,  plain chunks: 4,  ratio: 1
chunk 173: saved -2 bytes (compressed=43 bytes, original=41 bytes in message). Factor: 1.04878
   pointers: 0,  plain chunks: 1,  ratio: 0
chunk 174: saved 12 bytes (compressed=33 bytes, original=45 bytes in message). Factor: 0.733333
   pointers: 5,  plain chunks: 2,  ratio: 2.5
chunk 175: saved 10 bytes (compressed=29 bytes, original=39 bytes in message). Factor: 0.74359
   pointers: 4,  plain chunks: 1,  ratio: 4
chunk 176: saved 7 bytes (compressed=21 bytes, original=28 bytes in message). Factor: 0.75
   pointers: 3,  plain chunks: 1,  ratio: 3
chunk 177: saved 7 bytes (compressed=24 bytes, original=31 bytes in message). Factor: 0.774194
   pointers: 3,  plain chunks: 1,  ratio: 3
chunk 178: saved 11 bytes (compressed=31 bytes, original=42 bytes in message). Factor: 0.738095
   pointers: 4,  plain chunks: 1,  ratio: 4
chunk 179: saved 8 bytes (compressed=15 bytes, original=23 bytes in message). Factor: 0.652174
   pointers: 3,  plain chunks: 0,  ratio: 3
chunk 180: saved 7 bytes (compressed=18 bytes, original=25 bytes in message). Factor: 0.72
   pointers: 3,  plain chunks: 1,  ratio: 3
chunk 181: saved 11 bytes (compressed=14 bytes, original=25 bytes in message). Factor: 0.56
   pointers: 4,  plain chunks: 0,  ratio: 4
chunk 182: saved 10 bytes (compressed=23 bytes, original=33 bytes in message). Factor: 0.69697
   pointers: 4,  plain chunks: 1,  ratio: 4
chunk 183: saved 4 bytes (compressed=32 bytes, original=36 bytes in message). Factor: 0.888889
   pointers: 2,  plain chunks: 2,  ratio: 1
chunk 184: saved 1 bytes (compressed=23 bytes, original=24 bytes in message). Factor: 0.958333
   pointers: 1,  plain chunks: 2,  ratio: 0.5
chunk 185: saved 7 bytes (compressed=20 bytes, original=27 bytes in message). Factor: 0.740741
   pointers: 3,  plain chunks: 1,  ratio: 3
````

Let's get a summary to compare it with the previous results:

```
$ cat log | awk 'BEGIN{sum=0;lines=0;} {sum = sum + $2 ; lines++;} END{print "**saved "sum/lines" bytes on average, i.e., "sum " bytes within " lines" lines";}'
```

Output:

```
**saved 11.1784 bytes on average, i.e., 2068 bytes within 185 lines
```

As can be seen, the compression factor was much better here.
