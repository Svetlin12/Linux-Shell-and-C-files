#!/bin/bash

if [ $# -ne 3 ]; then
	echo "error: expected 3 arguments"
	exit 1
fi

SRC=$1
DST=$2
ABC=$3

if [ ! -d "${SRC}" ]; then
	echo "error: ${SRC} is not a directory
	exit 2
fi

if [ ! -d "${DST}" ]; then
	echo "error: ${DST} is not a directory
	exit 3
fi

if [ $(find "${DST}" -mindepth 1 | wc -l) -ne 0 ]; then
	echo "error: $DST is not empty"
	exit 4
fi

if [ $(id -u) -ne 0 ]; then
	echo "error: expected root as caller"
	exit 5
fi

find "${SRC}" -mindepth 1 -maxdepth 1 -exec cp -rp {} "${DST}" \;
find "${DST}" -type f ! -name "*${ABC}*" -exec rm {} \;
find "${DST}" -type d -empty -delete
