#!/bin/bash

if [ $# -ne 1 -a $# -ne 2 ]; then
        echo "usage: ./task14.sh dirname [filename]"
	exit 1
fi

DIRNAME=$1

if [ ! -d "${DIRNAME}" ]; then
	echo "${DIRNAME} is not a directory"
	exit 2
fi

if [ ! -r "${DIRNAME}" ]; then
	echo "${DIRNAME} is not readable"
	exit 3
fi

if [ $# -eq 2 ]; then
	FILENAME=$2
fi

if [ -n "${FILENAME}" -a ! -f "${FILENAME}" ]; then
	echo "${FILENAME} is not a file"
	exit 4
fi

if [ -n "${FILENAME}" -a ! -w "${FILENAME}" ]; then
	echo "${FILENAME} is not readable"
	exit 5
fi

BROKENSYMLINKS=$(find "${DIRNAME}" -type l ! -exec test -e {} \; -print 2>/dev/null | wc -l);
SYMLINKS=$(find "${DIRNAME}" -type l -exec test -e {} \; -print 2>/dev/null)
DESTINATION=$(find "${DIRNAME}" -type l -exec test -e {} \; -print 2>/dev/null | xargs -I {} readlink {});

COMBINED=$(paste <(echo "${SYMLINKS}") <(echo "${DESTINATION}"))

while read _SYMLINK _DEST; do
	if [ -n "${FILENAME}" ]; then
		echo "${_SYMLINK} -> ${_DEST}" >> "${FILENAME}"
	else
		echo "${_SYMLINK} -> ${_DEST}"
	fi
done < <(echo "${COMBINED}") 

if [ -n "${FILENAME}" ]; then
	echo "Broken symlinks: ${BROKENSYMLINKS}" >> "${FILENAME}"
else
	echo "Broken symlinks: ${BROKENSYMLINKS}"
fi
