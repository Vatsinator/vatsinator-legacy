#!/bin/sh -e

#
# This script is under construction.
#

dist=$1
branch=$2

if [ -z "$dist" ]; then
	echo "Usage: $0 <dist> [branch]"
	echo "Example: $0 precise master"
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
if [ -z "$branch" ]; then
	git clone $vatsinator_git $builddir
else
	git clone $vatsinator_git -b $branch $builddir
fi

mkdir $builddir/bin
cd $builddir/bin
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release


echo "Cleaning up..."

# cleanup
cd $tmpdir/..
rm -rf $tmpdir

echo "Done."

