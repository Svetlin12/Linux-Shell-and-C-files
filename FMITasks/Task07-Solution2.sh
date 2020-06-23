#!/bin/bash

if [ $# -ne 0 ]; then
	echo "Invalid argument count. Usage: $0"
	exit 1
fi

while read LINE; do
	if [ "$(egrep -xc "[-]?[1-9][0-9]*" <(echo "${LINE}"))" -eq 1 ]; then
		NUMS=$(echo "${NUMS} ${LINE}")
	fi
done

while read LINE; do
	SUM=$(echo "${LINE}" | tr " " "+" |  bc) 
	SUMS=$(echo "${SUMS} ${SUM}")
done < <(echo "${NUMS}" | cut -c 2- | tr " " "\n" | sed -E 's/^-//' | sed -E 's/([0-9])/\1 /g')

SUMS=$(echo "${SUMS}" | cut -c 2- | tr " " "\n")
NUMS=$(echo "${NUMS}" | cut -c 2- | tr " " "\n")

TOTAL=$(paste <(echo "${NUMS}") <(echo "${SUMS}"))

echo "${TOTAL}" | awk '{ if ($2 >= maxSum) {maxSum=$2; if (minNumber > $1) {minNumber = $1} } } END{print minNumber}' 
