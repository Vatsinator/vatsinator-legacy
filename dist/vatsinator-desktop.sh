#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=$1
Type=Application
Name=Vatsinator
GenericName=Vatsim monitor
GenericName[pl]=Monitor sieci Vatsim
Comment=A simple Vatsim monitor
Comment[pl]=Monitor sieci Vatsim
TryExec=$2/bin/vatsinator
Exec=$2/bin/vatsinator &
Categories=Application;Network;Qt;
Icon=vatsinator
EOF
