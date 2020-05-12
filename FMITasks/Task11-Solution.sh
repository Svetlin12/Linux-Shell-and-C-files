#!/bin/bash

if [ $(id -u) -ne 0 ]; then
	echo "expected root as caller"
	exit 1
fi

if [ $# -ne 1 ]; then
	echo "usage: ./task17.sh USERNAME"
	exit 2
fi

FOO=$1

if [ $(grep -c "${FOO}" <(cat /etc/passwd | cut -d : -f 1)) == 0 ]; then
	echo "invalid username"
	exit 3
fi

### a)

FOOPCOUNT=$(ps -u "${FOO}" --no-headers | wc -l)

while read _USER; do
	USERPCOUNT=$(ps -u "${_USER}" --no-headers | wc -l)
	
	if [ "${USERPCOUNT}" -gt "${FOOPCOUNT}" ]; then
		echo "${_USER}"
	fi

done < <(cat /etc/passwd | cut -d : -f 1)

### b)

AVG=$(awk '{cnt++; time += $1} END{ print time/cnt }' <(ps -e -o etimes=))
echo "${AVG}"

### c)

while read PID PROCTIME; do

	if [ $(echo "${PROCTIME} > $(echo "${AVG} * 2" | bc)" | bc) -eq 1 ]; then
		echo "pid: ${PID} etimes: ${PROCTIME}"
		# kill -9 ${PID}
		# sleep 2
		# kill -15 ${PID}
	fi

done < <(ps -u "${FOO}" -o pid=,etimes=)
