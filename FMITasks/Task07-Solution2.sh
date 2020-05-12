#!/bin/bash

NUMBERS=""

while read LINE; do
        if [ $(echo "${LINE}" | egrep "^-?[0-9]+$" | grep -vc "\.") -eq 1 ]; then
                NUMBERS="${NUMBERS} ${LINE}"
        fi
done

NUMBERS=$(echo "${NUMBERS}" | cut -c 2- | tr " " "\n" | sort -n | uniq)

NUMBERSANDSUMS=""

for i in $NUMBERS; do
        ABSOLUTE_VALUE=$(echo "${i}" | egrep -o "[0-9]+")
        DIGITSUM=$(echo "${ABSOLUTE_VALUE}" | sed -E "s/([0-9])/\1 /g" | tr " " "\n" | awk '{sum+=$1} END{print sum}')
        NUMBERSANDSUMS="${NUMBERSANDSUMS} ${i}+${DIGITSUM}"
done

NUMBERSANDSUMS=$(echo "${NUMBERSANDSUMS}" | tr " " "\n" | sort -t "+" -k2 -rn)
TARGETNUMS=""

MIN=0
for i in $NUMBERSANDSUMS; do
        SUM=$(echo "${i}" | cut -d "+" -f2)
        NUMBER=$(echo "${i}" | cut -d "+" -f1)
        if [ ${MIN} -eq 0 ]; then
                MIN=$SUM
                TARGETNUMS="${TARGETNUMS} ${NUMBER}"
        elif [ ${MIN} -gt ${SUM} ]; then
                break
        elif [ ${MIN} -eq ${SUM} ]; then
                TARGETNUMS="${TARGETNUMS} ${NUMBER}"
        fi
done

echo "${TARGETNUMS}" | cut -c 2- | tr " " "\n" | sort -rn | tail -n 1
