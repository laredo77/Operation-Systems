#!/bin/bash
# Itamar Laredo 311547087

# In case script recive less or more then 4 arguments --> print erorr and exit.
if [ "$#" -lt 4 ];
then
	printf "Not enough parameters\n"
	exit
else
	# Sort all folders by lexicographic name in the current folder
	# as well as in depth folders, and store it in var 'container'.
	container=$(find $1 -type d | LC_ALL=C sort)
	for i in $container
	do
		# Each folder is sent as an argument to the ex21.sh script and it keep its output in the variable 'file'.
		content=$(bash ./ex21.sh $i $2 $3)
		printf "%s\n" "$content" | while IFS= read -r line;
		do
			# Read the specific file and look for the requested word.
			# If the line length is greater-equal than the number given by the user  --> continue.
			if [ $(printf "%s\n" "$line" | wc -w) -ge $4 ]
			then
				# Print the line according to regex
				printf "%s\n" "$line" | grep -v '^$'
			fi
		done
	done
fi