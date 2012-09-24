#!/bin/sh -e
#
# This script is used by cmake to copy needed libs to .app directory
# and create the dmg file.
#


qtlibs="QtCore QtGui QtOpenGL QtXml QtSvg QtNetwork"
app=$1
qtdir=$2
binexec=vatsinator

if [ -z "$app" -o -z "$qtdir" ]; then
	echo "Usage: $0 <appfile> <qtdir>"
	exit 1
fi

rm -rf $app/Contents/Frameworks
mkdir -p $app/Contents/Frameworks

for i in $qtlibs; do
	echo "Copying $i..."
	cp -R $qtdir/$i.framework $app/Contents/Frameworks

	install_name_tool -id @executable_path/../Frameworks/$i.framework/Versions/Current/$i	\
		$app/Contents/Frameworks/$i.framework/Versions/Current/$i
	install_name_tool -change $qtdir/$i.framework/Versions/Current/$i			\
		@executable_path/../Frameworks/$i.framework/Versions/Current/$i			\
		$app/Contents/MacOS/$binexec
	
	if [ "$i" -ne "QtCore" ]; then
		install_name_tool -change $qtdir/QtCore.framework/Versions/Current/QtCore	\
		@executable_path/../Frameworks/QtCore.framework/Versions/Current/QtCore		\
		$app/Contents/Frameworks/$i.framework/Versions/Current/$i

		if [ "$i" -ne "QtGui" ]; then
			install_name_tool -change $qtdir/QtGui.framework/Versions/Current/QtGui	\
			@executable_path/../Frameworks/QtGui.framework/Versions/Current/QtGui	\
			$app/Contents/Frameworks/$i.framework/Versions/Current/$i
		fi
	
	fi

done

echo
echo "Cleaning up..."
rm -rf $app/Contents/Frameworks/*/*/*/*_debug
rm -rf $app/Contents/Frameworks/*/*/*/Headers

echo
echo "Done."

