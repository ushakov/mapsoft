all: make_map_gk update_map

CPPFLAGS=-I../
make_map_gk: ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a
update_map: ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck zn.cpp 

key_test: zn.cpp ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a

