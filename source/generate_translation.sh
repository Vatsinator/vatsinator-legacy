#!/usr/bin/env bash

lupdate -recursive . -source-language en_GB -target-language $1 -ts translations/vatsinator-$1.ts

