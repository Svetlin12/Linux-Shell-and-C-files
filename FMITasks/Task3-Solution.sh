#!/bin/bash

if [ $# -ne 3 ]; then
	echo "Error: expected 3 arguments"
	exit 1
fi

if [ ! -f "$1" ]; then
	echo "Error: expected file as first argument"
	exit 2
fi

FILENAME="$1"

# get lines like FOO=76
ARG1=$(cat "${FILENAME}" | grep -w "$2")
ARG2=$(cat "${FILENAME}" | grep -w "$3")

# prepare the keys for a for cycle
OLDKEYS1=$(echo "${ARG1}" | egrep -o "=.*" | cut -c 2- | sed -E 's/(.)/\1 /g' | tr -s " ")

# no need to prepare those keys for a for cycle
OLDKEYS2=$(echo "${ARG2}" | egrep -o "=.*" | cut -c 2-)

# for each key in ARG1 remove it from the keys in ARG2
for char in ${OLDKEYS1}; do
	NEWKEYS2=$(sed "s/${char}//g" <(echo "${OLDKEYS2}"))
	OLDKEYS2=${NEWKEYS2}
done

# remove the trailing whitespace at the beginning when ARG2 had multiple keys
NEWKEYS2=$(sed -E 's/^ //' <(echo "${NEWKEYS2}")) 

# make the change
sed -i "" "s/${ARG2}/$3=${NEWKEYS2}/" "${FILENAME}"
