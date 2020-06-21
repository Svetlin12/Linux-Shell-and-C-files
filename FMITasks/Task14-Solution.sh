#!/bin/bash

if [ $# -eq 0 ] || [ $# -gt 2 ]; then
	echo "Invalid argument count. Usage: $0 (dirname) (optional: filename)"
	exit 1
fi

DIR=${1}
FILE=${2}

if [ ! -d "${DIR}" ] || [ ! -r "${DIR}" ]; then
	echo "${DIR} is not a directory or is not readable"
	exit 2
fi

if [ -n "${FILE}" ] && [ ! -w "${FILE}" ]; then
	echo "${FILE} is not writable or does not exist"
	exit 3
fi

BROKEN=$(find "${DIR}" -type l ! -exec test -e {} \; -print 2>/dev/null | wc -l)
SYMLINKS=$(find "${DIR}" -type l -exec test -e {} \; -printf "%f \n" 2>/dev/null)
DEST=$(find -L "${DIR}" -xtype l -exec test -e {} \; -printf "%p\n" 2>/dev/null);

COMBINED=$(paste <(echo "${SYMLINKS}") <(echo "${DEST}"))

while read _SYMLINK _DEST; do
	if [ -n "${FILE}" ]; then
		echo "${_SYMLINK} -> ${_DEST}" >> "${FILE}"
	else
		echo "${_SYMLINK} -> ${_DEST}"
	fi
done < <(echo "${COMBINED}")

if [ -n "${FILE}" ]; then
	echo "Broken symlinks: ${BROKEN}" >> "${FILE}"
else
	echo "Broken symlinks: ${BROKEN}"
fi
