#!/bin/bash

if [ $# -ne 1 ]; then
	echo "usage: ./task16.sh LOGDIR"
	exit 1
fi

LOGDIR=$1

if [ ! -d "${LOGDIR}" ]; then
	echo "${LOGDIR} is not a directory"
	exit 2
fi

if [ ! -r "${LOGDIR}" ]; then
	echo "${LOGDIR} is not readable"
	exit 3
fi

while read FRIEND; do

	MESSAGECOUNT=$(find "${LOGDIR}" -type f | grep -w "${FRIEND}" | xargs -I {} wc -l {} | cut -d " " -f 1 | awk 'BEGIN {sum = 0} { sum += $1 } END { print sum }')
	LINES=$(echo "${LINES}${FRIEND} ${MESSAGECOUNT}\n")	
	
done < <(find "${LOGDIR}" -mindepth 3 -maxdepth 3 -printf "%f\n" | sort | uniq)

echo -e "${LINES}" | sort -t " " -k 2 -rn | head -n 10
