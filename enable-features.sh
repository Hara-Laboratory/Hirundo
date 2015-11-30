#!/bin/bash

function show_usage () {
	echo "USAGE: $0 <input-file> <output-file> <feature>..."
}

function disable_all_features () {
	sed -e 's!\(//\)\?\(.* //?.*?\)$!//\2!g'
}

function enable_feature () {
	sed -e 's!\(//\)\?\(.* //?'"$(features_to_string "$@")"'?\)$!\2!g'
}

function features_to_string () {
	printf "\\("
	while [ "$#" -gt 1 ] ; do
		printf "%s\\|" $1
		shift
	done
	if [ "$#" -gt 0 ] ; then
		printf "%s" $1
	fi
	printf "\\)"
}

function with_features () {
	disable_all_features | enable_feature "$@"
}

if [ "$#" -lt 2 ] ; then
	show_usage
	exit 1
fi

input_file=$1
output_file=$2

shift
shift

cat "${input_file}" | with_features "$@" > "${output_file}"


