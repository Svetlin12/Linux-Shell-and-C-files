#!/bin/bash

if [ $# -ne 1 -a $# -ne 2 ]; then
	echo "error: expected one or two arguments as input"
	exit 1
fi

if [ ! -d "$1" ]; then
	echo "error: directory does not exist"
	exit 2
fi

if [ ! -r "$1" ]; then
	echo "error: directory is not readable"
	exit 3
fi

DIRECTORY=$1
NUMBER=$2

if [ $# -eq 2 ]; then
	while read NUMHARDLINKS FILENAME; do
		if [ ${NUMHARDLINKS} -ge ${NUMBER} ]; then
			echo "${FILENAME}"
		fi
	done < <(find "${DIRECTORY}" -type f 2>/dev/null -printf "%n %p\n")   
elif [ $# -eq 1 ]; then
	find -L "${DIRECTORY}" -type l 2>/dev/null -print0 | xargs -0 -I {} basename {}
fi
