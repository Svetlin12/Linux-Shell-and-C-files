#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Invalid number of arguments! Usage: ./$0 (number)"
	exit 1
fi

NUM=$1

if [ $(egrep -c "^[1-9][0-9]*" <(echo "${NUM}")) -eq 0 ]; then
	echo "Argument should be a positive number"
	exit 2
fi

if [ $(id -u) -ne 0 ]; then
	echo "Expected root as caller"
	exit 3
fi

while read _USER; do
	RSS_MAX=$(awk 'BEGIN{sum=0} { sum += $1; if (max < $1) {max = $1; pid=$2} } END{print sum " " max}' <(ps -u "${_USER}" -o rss=,pid=))
	TOTAL_RSS=$(echo "${RSS_MAX}" | cut -d " " -f1)
	MAX_PID=$(echo "${RSS_MAX}" | cut -d " " -f3)
	if [ "${TOTAL_RSS}" -gt "${NUM}" ]; then
		#kill -15 "${MAX_PID}"
		#sleep 2
		#kill -9 "${MAX_PID}"
	fi
done < <(cut -d : -f1 /etc/passwd)
