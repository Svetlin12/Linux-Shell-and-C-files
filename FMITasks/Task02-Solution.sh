#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Error: expected 1 argument!"
	exit 1
fi

if [ $(id -u) -ne 0 ]; then
	echo "Error: expected root as caller!"
	exit 2
fi

AIMEDAMOUNT=$1
ALLUSERS=$(ps -e -o user= | sort | uniq)

for USR in ${ALLUSERS}; do
	
	PROCESSCNT=$(ps -u "${USR}" -o rss= | sort -n | wc -l)
	TOTALSIZE=$(ps -u "${USR}" -o rss= | sort -n | awk '{SZ += $1} END{print SZ}')
	
	if [ ${TOTALSIZE} -gt ${AIMEDAMOUNT} ]; then
		TOKILL=$(ps -u  "${USR}" -o rss=,pid= | sort -n | tail -n 1 | awk '{print $2}')
		kill -s TERM ${TOKILL}
		sleep 3
		kill -s KILL ${TOKILL}
	fi

done	
