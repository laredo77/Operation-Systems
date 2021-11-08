#!/bin/bash
# Itamar Laredo 311547087

# In case script recive less or more then 3 arguments --> print erorr and exit.
if [ "$#" -lt 3 ];
then
	printf "Not enough parameters\n"
	exit
else
	# Sorting the current directory by file extensions as received and store it in var 'files'.
	files=$(find $1 -maxdepth 1 -type f -name "*.$2" | LC_ALL=C sort)
	for i in $files
	do
		# Go through the files with the relevant extension and print the lines that contain the requested word.
		grep -wh $3 -i $i
	done
fi 