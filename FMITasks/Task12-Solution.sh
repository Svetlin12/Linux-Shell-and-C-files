#!/bin/bash

while read _USER _HOME; do
	
	if [ ! -r "${_HOME}" ]; then
		continue
	fi

	if [ ! -d "${_HOME}" ]; then
		continue
	fi

	FILE=$(find "${_HOME}" -type f ! -name ".*" -printf "%T@ %f\n" 2>/dev/null | sort -t " " -k1 -n | tail -n 1)	

	if [ ! -n "${FILE}" ]; then
		continue
	fi

	RECENTFILES="${RECENTFILES}${_USER} ${FILE}\n"

done < <(cat /etc/passwd | cut -d : -f 1,6 | tr : " ")

echo -e "${RECENTFILES}" | sort -t " " -k2 -n | tail -n 1 | cut -d " " -f 1,3-
