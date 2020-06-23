#!/bin/bash

if [ $# -eq 0 ]; then
	echo "Invalid argument count. Usage: $0 [-n N] (logfile.log)..."
	exit 1
fi

if [ $1 = "-n" -a $(egrep -c "^[0-9]+$" <(echo "$2")) -ne 1 ]; then
	echo "Invalid number following -n option"
	exit 2
fi

OUTPUT=$(mktemp)
N=10

for FILE; do
	
	[ "${FILE}" = "-n" ] && continue
	[ $(egrep -c "^[0-9]+$" <(echo "${FILE}")) -eq 1 ] && { N=$2; } && continue
	[ ! -f "${FILE}" ] && continue
	[ ! -r "${FILE}" ] && continue

	IDF=$(echo "${FILE}" | sed -E 's/(.*).log/\1/')
	sed -E "s/([1-9][0-9]{3}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}) (.*)/\1 ${IDF} \2/" "${FILE}" | tail -"${N}" >> "${OUTPUT}"

done  

cat "${OUTPUT}" | sort

rm -- "${OUTPUT}"
