#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=0.1_alpha3
Type=Application
Name=Vatsinator
GenericName=Vatsim monitor
Comment=A simple Vatsim monitor
TryExec=$1/bin/vatsinator
Exec=$1/bin/vatsinator &
Categories=Application;Network;Qt;
Icon=vatsinator
EOF
