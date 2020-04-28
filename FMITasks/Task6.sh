#!/bin/bash

if [ $# -ne 2 ]; then
	echo "error: expected 2 arguments as input"
	exit 1
fi

if [ ! -d "$1" ]; then
	echo "error: $1 is not a directory name"
	exit 2
fi 

if [ ! -r "$1" ]; then
	echo "error: $1 is not a readable directory"
	exit 3
fi

DIRECTORY=$1
ARCH=$2

find "${DIRECTORY}" -maxdepth 1 -mindepth 1 -type f -exec basename {} \; | grep -E "vmlinuz-[0-9]+\.[0-9]+\.[0-9]+-${ARCH}" | sort -V | tail -n 1
