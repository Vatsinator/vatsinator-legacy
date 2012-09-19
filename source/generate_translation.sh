#!/usr/bin/env bash

if [ -z "$1" ]; then
	echo "No locale given!"
	exit
fi

lupdate -recursive . -source-language en_GB -target-language $1 -ts translations/vatsinator-$1.ts

