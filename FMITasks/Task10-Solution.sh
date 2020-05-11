#!/bin/bash

if [ "$(id -u)" -ne 0 ]; then
	echo "Expected root as caller";
	exit 1;
fi

while read _USER _HOME; do

	if [ ! -d "${_HOME}" ]; then
		echo "${_USER} does not have a home directory"
		continue;	
	fi

	if sudo -u ${_USER} [ ! -w "${_HOME}" ]; then
		echo "${_USER} cannot write to his/her home directory"
		continue;
	fi

done < <(cat /etc/passwd | cut -d : -f 1,6 | tr : " ")
