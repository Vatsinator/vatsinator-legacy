#!/bin/sh -e

tag=$1
dist=$2

if [ -z "$dist" ]; then
	echo "Usage: $0 <tag> <dist>"
	echo "Example: $0 0.1 precise"
	exit 1
fi

vatsinator_ppa=ppa:michal-k93/vatsim
vatsinator_git=git://github.com/Garrappachc/Vatsinator.git

tmpdir=`pwd`/work
builddir=vatsinator

echo " * Temporary directory: $tmpdir"
echo " * Build directory: $tmpdir/$builddir"

echo
echo " ==> Downloading source..."
echo

# prepare
mkdir -p $tmpdir

# clone source code
cd $tmpdir
git clone $vatsinator_git $builddir
cd $builddir
git checkout $tag

echo
echo " ==> Preparing files..."
echo

mkdir bin
cd bin
cmake .. -DDEB_DIST=$dist -DWITH_DEBIAN=ON
rm -rf *

echo
echo " ==> Creating packages..."
echo

cd $tmpdir/$builddir/dist
./maketarball.sh
mv $tmpdir/$builddir/dist/*.orig.tar.gz $tmpdir/
rm $tmpdir/$builddir/dist/*.tar.gz
rm -rf $tmpdir/$builddir/.git

cd $tmpdir/$builddir
dpkg-buildpackage -S -k63EA79F5

echo
echo " ==> Uploading package..."
echo

cd $tmpdir
dput $vatsinator_ppa *_source.changes

echo
echo " ==> Cleaning up..."
echo

# cleanup
cd $tmpdir/..
rm -rf $tmpdir

echo
echo " ==> Done."

