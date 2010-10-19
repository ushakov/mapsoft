CPPFLAGS = -O1 -I../core/

all: test_types gs_bbx_test

test_types: ../core/geo/geo_types.cpp
gs_bbx_test: ../core/fig/libfig.a ../core/utils/libutils.a

LDFLAGS=-lpng -ljpeg -ltiff
test_ptshift: ../geo/libgeo.a ../loaders/libloaders.a ../utils/libutils.a ../jeeps/libjeeps.a ../lib2d/lib2d.a
