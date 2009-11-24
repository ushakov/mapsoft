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
BuildRequires: libgd2-devel libgtkmm2-devel libjpeg-devel libtiff-devel
BuildRequires: libusb-compat-devel libyaml-devel proj-devel python-devel
BuildRequires: scons swig

%description
mapsoft - programs for working with maps and geodata

%prep
%setup -q

%build
scons

%install
scons --prefix=%buildroot install

%files
%_bindir/*
%_mandir/man?/*
%dir %_datadir/mapsoft
%_datadir/mapsoft/*

%changelog
* Thu Nov 19 2009 Vladislav Zavjalov <slazav@altlinux.org> 20091119-alt1
- first build for altlinux

