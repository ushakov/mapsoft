CPPFLAGS = -O1 -I../core/

all: test_types options_test options_read read_conf_test gs_bbx_test

test_types: ../core/libgeo/geo_types.cpp
options_test: ../core/utils/options.cpp
options_read: ../core/utils/options.cpp
read_conf_test: ../core/utils/options.cpp ../core/utils/read_conf.cpp
gs_bbx_test: ../core/libfig/libfig.a ../core/utils/libutils.a
read_conf_test strvec_test: ../core/utils/strvec.cpp

test_mp: ../libmp/libmp.a ../utils/libutils.a

LDFLAGS=-lpng -ljpeg -ltiff
test_ptshift: ../libgeo/libgeo.a ../loaders/libloaders.a ../utils/libutils.a ../jeeps/libjeeps.a ../lib2d/lib2d.a
