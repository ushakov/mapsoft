Name: mapsoft
Version: 20091119
Release: alt1
License: GPL
Summary: mapsoft - programs for working with maps and geodata
Group: Sciences/Geosciences
Url: http://github.org/ushakov/mapsoft
Packager: Vladislav Zavjalov <slazav@altlinux.org>

Source: %name-%version.tar

BuildRequires: boost-devel gcc-c++ libcurl-devel
BuildRequires: libcairomm-devel libgtkmm2-devel
BuildRequires: libpng-devel libjpeg-devel libtiff-devel
BuildRequires: libusb-devel libyaml-devel libxml2-devel proj-devel
BuildRequires: python-devel scons swig transfig ImageMagick-tools

%package vmap
Summary: mapsoft-vmap - programs for working with vector maps
Group: Sciences/Geosciences
Requires: %name = %version-%release

%description
mapsoft - programs for working with maps and geodata

%description vmap
mapsoft-vmap - programs for working with vector maps

%prep
%setup -q

%build
scons -Q minimal=1

%install
scons -Q minimal=1 -Q prefix=%buildroot install

%files
%_bindir/mapsoft_convert
%_bindir/mapsoft_mapview
%_mandir/man1/mapsoft_convert.1

%files vmap
%_bindir/vmap_copy
%_bindir/vmap_render
%dir %_datadir/mapsoft
%_datadir/mapsoft/*

%changelog
* Thu Nov 19 2009 Vladislav Zavjalov <slazav@altlinux.org> 20091119-alt1
- first build for altlinux

