#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Invalid number of arguments!"
	exit 1
fi

DIR_PATH="${1}"

if [ ! -d "${DIR_PATH}" ]; then
	echo "Argument is not a directory!"
	exit 2
elif [ ! -r "$DIR_PATH" ]; then
	echo "Directory is not readable!"
	exit 3
fi

find -L "${DIR_PATH}" -type l 2>/dev/null -print0 | xargs -0 -I {} basename {}
svetlinpopivanov@Air~$ cat task14.sh
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
