#!/bin/bash
# Itamar Laredo

# Arguments varibale defualt initialize by false value.
NAME=0
VERSION=0
PRETTY_NAME=0
HOME_URL=0
SUPPORT_URL=0
STATIC_HOSTNAME=0
ICON_NAME=0
MACHINE_ID=0
BOOT_ID=0
VIRTUALIZATION=0
KERNEL=0
ARCHIT=0

# Initialize system array and host array to store the specific vars.
sys_arr=()
host_arr=()

# In-case user choose system option.
if [ "$1" == "system" ];
then
	# Goes through the arguments in a loop and uses a switch-case to
	# light the flags of the desired arguments. In addition it keeps it in the appropriate array.
	for arg in "$@"
	do
		case $arg in
			-n | --name)
			if [ "$NAME" = 0 ]; then
				NAME=1
				sys_arr+=("NAME")
			fi
			;;

			-v | --version)
			if [ "$VERSION" = 0 ]; then
				VERSION=1
				sys_arr+=("VERSION")
			fi
			;;

			-p | --pretty_name)
			if [ "$PRETTY_NAME" = 0 ]; then
				PRETTY_NAME=1
				sys_arr+=("PRETTY_NAME")
			fi
			;;

			-h | --home_url)
			if [ "$HOME_URL" = 0 ]; then
				HOME_URL=1
				sys_arr+=("HOME_URL")
			fi
			;;

			-su | --support_url)
			if [ "$SUPPORT_URL" = 0 ]; then
				SUPPORT_URL=1
				sys_arr+=("SUPPORT_URL")
			fi
			;;
	    esac
	done

	# Case no flags requested --> read the file and print it line by line.
	if [ "$NAME" -eq "0" ] && [ "$VERSION" -eq "0" ] && [ "$PRETTY_NAME" -eq "0" ] && [ "$HOME_URL" -eq "0" ] && [ "$SUPPORT_URL" -eq "0" ];
	then
		cat os-release | while IFS= read -r line;
		do
			printf "%s\n" "$line"
		done
	fi

	# Search for each flag in the file by using split and print the appropriate content for the desired flag.
	for i in "${sys_arr[@]}"
	do
		cat os-release | while IFS= read -r line;
		do
			prefix="$(cut -d "=" -f1 <<< "$line")"
			if [ "$prefix" = "$i" ]
			then
				suffix="$(cut -d "=" -f2 <<< "$line")"
				echo $suffix | tr -d '"'
			fi
		done
	done

# In-case user choose host option.
elif [ "$1" == "host" ];
then
	# Goes through the arguments in a loop and uses a switch-case to
	# light the flags of the desired arguments. In addition it keeps it in the appropriate array.
	for arg in "$@"
	do
		case $arg in
			-sh | --static_hostname)
			if [ "$STATIC_HOSTNAME" = 0 ]; then
				STATIC_HOSTNAME=1
				host_arr+=("Static hostname")
			fi
			;;

			-icn | --icon_name)
			if [ "$ICON_NAME" = 0 ]; then
				ICON_NAME=1
				host_arr+=("Icon name")
			fi
			;;

			-mid | --machine_id)
			if [ "$MACHINE_ID" = 0 ]; then
				MACHINE_ID=1
				host_arr+=("Machine ID")
			fi
			;;

			-bid | --boot_id)
			if [ "$BOOT_ID" = 0 ]; then
				BOOT_ID=1
				host_arr+=("Boot ID")
			fi
			;;

			-vir | --virtualization)
			if [ "$VIRTUALIZATION" = 0 ]; then
				VIRTUALIZATION=1
				host_arr+=("Virtualization")
			fi
			;;

			-ker | --kernel)
			if [ "$KERNEL" = 0 ]; then
				KERNEL=1
				host_arr+=("Kernel")
			fi
			;;

			-arc | --architecture)
			if [ "$ARCHIT" = 0 ]; then
				ARCHIT=1
				host_arr+=("Architecture")
			fi
			;;
	    esac
	done

	# Case no flags requested --> read the file and print it line by line.
	if [ "$STATIC_HOSTNAME" -eq "0" ] && [ "$ICON_NAME" -eq "0" ] && [ "$MACHINE_ID" -eq "0" ] && [ "$BOOT_ID" -eq "0" ] && [ "$VIRTUALIZATION" -eq "0" ] && [ "$KERNEL" -eq "0" ] && [ "$ARCHIT" -eq "0" ];
	then
		cat hostnamectl | while IFS= read -r line;
		do
			printf "%s\n" "$line"
		done
	fi

	# Search for each flag in the file by using split and print the appropriate content for the desired flag.
	for i in "${host_arr[@]}"
	do
		cat hostnamectl | while IFS= read -r line;
		do
			prefix="$(cut -d ":" -f1 <<< "$line")"
			prefix=$(echo $prefix)
			if [ "$prefix" = "$i" ]
			then
				suffix="$(cut -d ":" -f2 <<< "$line")"
				echo $suffix | tr -d '"'
			fi
		done
	done
# Case invalid arguments --> print erorr and exit.
else
	echo "Invalid input"
	exit
fi
