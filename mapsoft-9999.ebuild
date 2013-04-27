# Copyright 1999-2013 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=1

EGIT_REPO_URI="git://github.com/ushakov/${PN}.git"
EGIT_BRANCH="master"

inherit git-2 python scons-utils

DESCRIPTION="Programs for working with maps and geodata"
HOMEPAGE="http://slazav.mccme.ru/prog/mapsoft.htm"
SRC_URI=""

LICENSE="GPL"
SLOT="0"
KEYWORDS=""
IUSE=""

DEPEND="app-arch/zip
	app-text/ghostscript-gpl
	dev-cpp/cairomm
	>=dev-cpp/gtkmm-2.4:2.4
	dev-lang/perl
	dev-lang/python
	dev-lang/swig
	dev-libs/boost
	dev-libs/libusb
	dev-libs/libyaml
	dev-libs/libxml2
	dev-util/scons
	media-gfx/imagemagick
	media-gfx/transfig
	media-libs/giflib
	media-libs/libjpeg-turbo
	media-libs/libpng
	media-libs/netpbm
	media-libs/tiff
	net-misc/curl
	sci-libs/proj
	sys-devel/m4
	sys-libs/zlib
	virtual/pkgconfig
	x11-libs/pixman"
RDEPEND="${DEPEND}"

src_compile() {
    escons -j1 -Q prefix="${D}" -Q minimal=1
}

src_install() {
	escons -Q prefix="${D}" -j1 install
#    dodoc txt || die
}
pkg_postinst() {
	elog ""
	elog "See the home page (${HOMEPAGE}) for more info."
	elog ""
}

