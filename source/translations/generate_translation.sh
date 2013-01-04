#!/usr/bin/env bash

unamestr=`uname`
if [[ "$unamestr" == "Linux" ]] ; then
	lupdate_exec=lupdate
elif [[ "$unamestr" == "FreeBSD" ]] ; then
	lupdate_exec=lupdate-qt4
fi

if [ -z "$1" ]; then
	echo "No locale given!"
	exit
fi

$lupdate_exec -recursive ../ -source-language en_GB -target-language $1 -ts vatsinator-$1.ts

