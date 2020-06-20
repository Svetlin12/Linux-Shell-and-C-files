#!/bin/bash

if [ $# -ne 1 ]; then
	echo "usage: ./$0 (dirname)"
	exit 1
fi

DIRECTORY=$1

if [ ! -d $1 ] || [ ! -r ${DIRECTORY} ]; then
	echo "$1 is not a directory or is not readable"
	exit 2
fi

find ${DIRECTORY} -type l -printf "%Y %f\n" 2>/dev/null | egrep "^[^nl]" | cut -d " " -f2 
