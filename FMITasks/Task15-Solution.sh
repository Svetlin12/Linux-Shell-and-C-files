#!/bin/bash

if [ $(id -u) -ne 0 ]; then
	echo "expected root as caller"
	exit 1
fi

SIZEROOT=$( ps -u "root" -o rss= | awk '{ total += $1 } END { print total }')	

while read _USER _HOME; do

	if [ "${_USER}" == "root" ]; then
		continue
	fi

	[ ! -d "${_HOME}" ] || [ "$(stat -c "%U" "${_HOME}" )" != "${_USER}" ] || [ "$(stat -c "%A" "${_HOME}" | cut -c 3)" != "w" ] || continue

	SIZEUSER=$( ps -u "${_USER}" -o rss= | awk 'BEGIN { total = 0 } { total += $1 } END { print total }')

	if [ "${SIZEROOT}" -lt "${SIZEUSER}" ]; then
		killall -u "${_USER}"
	        sleep 2
		killall -u "${_USER}" -s SIGKILL	
	fi

done < <(cat /etc/passwd | cut -d : -f 1,6 | tr : " ")
