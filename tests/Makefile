CPPFLAGS = -O1

all: test_types options_test options_read read_conf_test gs_bbx_test

test_types: ../libgeo/geo_types.cpp
options_test: ../utils/options.cpp
options_read: ../utils/options.cpp
read_conf_test: ../utils/options.cpp ../utils/read_conf.cpp
gs_bbx_test: ../libfig/libfig.a ../utils/libutils.a

test_mp: ../libmp/libmp.a ../utils/libutils.a

LDFLAGS=-lpng -ljpeg -ltiff
test_ptshift: ../libgeo/libgeo.a ../loaders/libloaders.a ../utils/libutils.a ../jeeps/libjeeps.a ../lib2d/lib2d.a
