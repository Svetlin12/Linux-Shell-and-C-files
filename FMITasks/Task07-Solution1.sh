#!/bin/bash

NUMBERS=""

while read LINE; do
        if [ $(echo "${LINE}" | egrep "^-?[0-9]+$" | grep -vc "\.") -eq 1 ]; then
                NUMBERS="${NUMBERS} ${LINE}"
        fi
done

NUMBERS=$(echo "${NUMBERS}" | cut -c 2- | tr " " "\n" | sort -n | uniq)

for i in $NUMBERS; do
        ABSOLUTE_VALUE=$(echo "${i}" | egrep -o "[0-9]+")
        if [ "${ABSOLUTE_VALUE}" ==  $(echo "${NUMBERS}" | tail -n 1) ]; then
                echo "${i}"
        fi
done
