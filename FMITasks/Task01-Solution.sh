#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Invalid number of arguments!"
	exit 1
fi

DIR_PATH="${1}"

if [ ! -d "${DIR_PATH}" ]; then
	echo "Argument is not a directory!"
	exit 2
elif [ ! -r "$DIR_PATH" ]; then
	echo "Directory is not readable!"
	exit 3
fi

find -L "${DIR_PATH}" -type l 2>/dev/null -print0 | xargs -0 -I {} basename {}
