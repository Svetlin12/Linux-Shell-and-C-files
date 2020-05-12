#!/bin/bash

if [ $# -ne 2 ]; then
	echo "usage: ./task27.sh filename(csv) filename"
	exit 1
fi

SOURCE=$1
DESTINATION=$2

if [ ! -f "${SOURCE}" ]; then
	echo "${SOURCE} is not a file"
	exit 2
fi

if [ ! -r "${SOURCE}" ]; then
	echo "${SOURCE} is not readable"
	exit 3
fi

if [ ! -f "${DESTINATION}" ]; then
	touch "${DESTINATION}"	
fi

CONTENT=$(cat "${SOURCE}" | sort -t , -k1 -n)

# clear file ${DESTINATION} content
truncate -s 0 "${DESTINATION}"

while read LINE; do
	TEXT=$(echo "${LINE}" | cut -d , -f 2-)
	TEXT_ID=$(echo "${LINE}" | cut -d , -f 1)
	if [ $(grep -c "${TEXT}" <(cat "${DESTINATION}")) -eq 0 ]; then
		echo "${LINE}" >> "${DESTINATION}"
	fi  
done < <(echo "${CONTENT}")
