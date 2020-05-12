#!/bin/bash

if [ $# -eq 0 ]; then
	echo "usage: ./task29.sh [-n N] FILE1 ..."
	exit 1
fi

if [ $1 == "-n" ]; then
	N=$2
	if [ $(egrep -c "^[0-9]+$" <(echo "${N}")) -eq 0 ]; then
		echo "expected a number after -n"
		exit 2
	fi
fi

for i; do

	if [ "${i}" == "-n" ]; then
		continue 2
	fi

	if [ ! -f "${i}" ]; then
		echo "${i} is not a file"
		continue
	fi

	if [ ! -r "${i}" ]; then
		echo "${i} is not readable"
		continue
	fi

	IDF=$(echo "${i}" | sed -E 's/(.*).log/\1/')
	LINES=$(cat "${i}" | sed -E "s/([0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}) (.*)/\1 ${IDF} \2\n/") 	

	OUTPUT="${OUTPUT}${LINES}\n"
done

if [ -z ${N} ]; then
	N=10
fi

echo -e "${OUTPUT}" | sort -t " " -k1,2 | tail -n "${N}"