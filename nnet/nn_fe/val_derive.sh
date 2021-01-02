#!/bin/sh

if [ x"$1" == x"" ]; then
    echo "$0 netname"
    echo "RIP DMR"
    exit 0
fi

CN=$(cat "$1.mnn" | head -n1 | cut -f1 -d' ')
CN=$(expr $CN + 1);

cat "$1.mni" | cut -f$CN -d' ' | sort | uniq > val_derive.out

NVALS=$(wc -l val_derive.out | cut -f1 -d' ')
NVALS=$(expr $NVALS \- 1)
STEP=$(echo "print 2.0 / $NVALS" | perl)

rm -f "$1.mnv"
C="-1.0"
IFS='
'
for item in $(cat val_derive.out); do
    echo "$item $C" >> "$1.mnv"
    C=$(echo "print $C + $STEP" | perl)
done

