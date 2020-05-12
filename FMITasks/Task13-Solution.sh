#!/bin/bash

if [ $(id -u) -ne 0 ]; then
	echo "expected root as caller"
	exit 1;
fi

###a)

echo "user pcount totalrss"

while read _USER; do

	awk -v user="${_USER}" '{ cnt++; size += $1 } END{ if (cnt != 0) {print user " " cnt " " size} else { print user " 0 0" } }' <(ps -u "${_USER}" -o rss=)

done < <(cat /etc/passwd | cut -d : -f 1)

###b)

while read _USER; do

	AVG=$(awk '{ cnt++; total += $1 } END{ if (cnt != 0) { print total/cnt } else { print "0" } }' <(ps -u "${_USER}" -o rss=))

	MAXPROC=$(awk 'BEGIN{ max=0; pid = -1 } { if (max < $2) { max = $2; pid = $1 } } END { print max " " pid}' <(ps -u "${_USER}" -o pid=,rss=))
	MAXRSS=$(echo "${MAXPROC}" | cut -d " " -f 1) 
	PID=$(echo "${MAXPROC}" | cut -d " " -f 2)

	if [ $(echo "${MAXRSS} > 2 * ${AVG}" | bc) -eq 1 ]; then
		kill -15 "${PID}"
		sleep 2
		kill -9 "${PID}"	
	fi

done < <(cat /etc/passwd | cut -d : -f 1)
