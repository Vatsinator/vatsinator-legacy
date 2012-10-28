#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=Vatsinator
GenericName=Vatsim monitor
GenericName[pl]=Monitor sieci Vatsim
Comment=A simple Vatsim monitor
Comment[pl]=Monitor sieci Vatsim
TryExec=$1/bin/vatsinator
Exec=$1/bin/vatsinator
Categories=Network;Qt;
Icon=vatsinator
EOF
