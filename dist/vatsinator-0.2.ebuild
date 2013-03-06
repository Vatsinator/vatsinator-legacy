# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=4
inherit cmake-utils

DESCRIPTION="A simple Vatsim monitor"
HOMEPAGE="https://github.com/Garrappachc/Vatsinator"
SRC_URI="http://vatsinator.garapich.pl/downloads/${P}.tar.gz"

LICENSE="GPL-3"
SLOT="0"
KEYWORDS="amd64 x86"
IUSE="debug"

DEPEND="
	>=dev-qt/qtgui-4.8.0
	>=dev-qt/qtopengl-4.8.0
	>=sys-devel/gcc-4.6
	dev-libs/quazip
	"
RDEPEND="${DEPEND}"

pkg_setup() {
	if use debug; then
		export CMAKE_BUILD_TYPE=Debug
	else
		export CMAKE_BUILD_TYPE=Release
	fi
}
