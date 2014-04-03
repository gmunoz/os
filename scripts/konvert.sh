#!/bin/bash

##########################################################################
# Author: Gabriel Munoz
# Date: August 20, 2004
# File: 
# Usage: This script will take as an argument a file of a C program to
#   convert for usage from user-space to kernel-space for vice-versa.
# Description: This is a simple script to modify a C source file and
#   change its code to suit either user or kernel space. It is best
#   suited to simply transferring a source file from the kernel proper
#   to user-space mainly for testing purposes. It should work to import
#   the file back into kernel space, but a backup is make in /tmp and
#   you should perform diffs to make sure only the relevant things were
#   changed.
##########################################################################

# setup any defaults or pre-processing commands
function usage
{
	echo "Usage: $0 [-h] [-i] [-e] filename"
	echo "  -e              Exports the given file from kernel-space to user-space."
	echo "  -h              Display help (what you are reading now)."
	echo "  -i              Imports the given file from user-space to kernel-space."
	echo ""
	echo "This script will convert a kernel-space C source file to a user-space"
	echo "C source file (export); or, it will convert a user-space C source"
	echo "file to a kernel-space file (import). Kernel headers will replaced"
	echo "by their user-space equivalents, and vice-versa. All user-space"
	echo "system calls will be replaced with their kernel equivalents."
}

function panic
{
	usage
	exit 1
}

EXPORT=0
IMPORT=0

# parse the command-line options
while getopts ":ehi" opt; do
	case $opt in
		e  ) EXPORT=1;;
		h  ) usage
		     exit 0;;
		i  ) IMPORT=1;;
		\? ) panic ;;
	esac
done

if [ $EXPORT -eq 0 ] && [ $IMPORT -eq 0 ]; then
	echo "error: You must specify a -i (import) or -e (export) flag"
	panic
fi

if [ $EXPORT -eq 1 ] && [ $IMPORT -eq 1 ]; then
	echo "error: You must specify either -i OR -e, and not both"
	panic
fi

shift $(($OPTIND - 1))

# A single parameter should exist, otherwise error and echo usage
if [ $# -ne 1 ]; then
	echo "error: You must specify a filename"
	panic
fi
file=$1

TMPFILE=`mktemp /tmp/konvert.XXXXXX` || panic
cp $file $TMPFILE
echo "A backup of the original file is stored at $TMPFILE"

# Remove/add headers based on whether this is an import or export.
if [ $IMPORT -eq 1 ]; then
	echo "Importing $file"
	# Remove the standard c library
	if grep -q "^#include.*stdlib\.h.*" $file; then
		echo -n "  Found stdlib.h and attempting to remove... "
		sed -i '/^#include.*[\<,\"]stdlib\.h[\>,\"].*/d' $file
		echo "Done!"
	fi
	# If NULL is used in the source, then add stddef.h
	if grep -q "NULL" $file; then
		echo -n "  $file uses NULL, adding stddef.h header..."
		sed -i '/^#include.*[\<,\"].*\.h[\>,\"].*/a#include <stddef.h>' $file
		echo "Done!"
	fi
	if grep -q "malloc" $file; then
		# Replace malloc with kmalloc and free with kfree
		echo -n "  Replacing malloc -> kmalloc and free -> kfree... "
		sed -i 's/malloc(/kmalloc(/g' $file
		sed -i 's/free(/kfree(/g' $file
		echo "Done!"
		# Add kmalloc.h
		echo -n "  Adding kmalloc.h..."
		sed -i '/^#include.*[\<,\"].*\.h[\>,\"].*/a#include <kmalloc.h>' $file
		echo "Done!"
	fi
elif [ $EXPORT -eq 1 ]; then
	echo "Exporting $file"
	# Remove stddef.h from kernel files as user space does not explicitly
	# need this.
	if grep -q "^#include.*stddef\.h.*" $file; then
		echo -n "  Found stddef.h and attempting to remove... "
		sed -i '/^#include.*[\<,\"]stddef\.h[\>,\"].*/d' $file
		echo "Done!"
	fi
	# Remvoe kmalloc.h - this is a kernel specific header
	if grep -q "^#include.*kmalloc\.h.*" $file; then
		echo -n "  Found kmalloc.h and attempting to remove... "
		sed -i '/^#include.*[\<,\"]kmalloc\.h[\>,\"].*/d' $file
		echo "Done!"
	fi
	# Replace malloc with kmalloc and free with kfree
	echo -n "  Replacing kmalloc -> malloc and kfree -> free... "
	sed -i 's/kmalloc(/malloc(/g' $file
	sed -i 's/kfree(/free(/g' $file
	echo "Done!"
fi
