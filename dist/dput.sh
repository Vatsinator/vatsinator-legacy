#!/bin/sh -e

#
# This script is under construction.
#

tag=$1
dist=$2

if [ -z "$dist" ]; then
	echo "Usage: $0 <tag> <dist>"
	echo "Example: $0 0.1 precise"
	exit 1
fi

vatsinator_ppa=ppa:michal-k93/vatsinator
vatsinator_git=git://github.com/Garrappachc/Vatsinator.git

tmpdir=`pwd`/work
builddir=vatsinator

echo "Temporary directory: $tmpdir"
echo "Build directory: $tmpdir/$builddir"

echo "Downloading source..."

# prepare
mkdir -p $tmpdir

# clone source code
cd $tmpdir
git clone $vatsinator_git $builddir
cd $builddir
git checkout $tag

mkdir bin
cd bin
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release


echo "Cleaning up..."

# cleanup
cd $tmpdir/..
rm -rf $tmpdir

echo "Done."

