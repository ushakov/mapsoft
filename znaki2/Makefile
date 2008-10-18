all: make_map_gk get_legend get_print fig2pnm get_figlib make_nom np2pt tx2pt

MAPSOFT_LIBS = ../libzn/libzn.a ../libgeo_io/libgeo_io.a ../libgeo/libgeo.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff -lusb ../yaml/yaml.cpp -lsyck ../libfig/libfig.a ../libmp/libmp.a ../utils/libutils.a ../lib2d/lib2d.a

CPPFLAGS=-I../ -g

make_map_gk: $(MAPSOFT_LIBS)
make_nom:    $(MAPSOFT_LIBS)

fig2pnm:     ../utils/pnm_shifter.cpp $(MAPSOFT_LIBS)
pnm_cnt:     ../utils/pnm_shifter.cpp

get_print:   $(MAPSOFT_LIBS)
np2pt:       $(MAPSOFT_LIBS)
tx2pt:       $(MAPSOFT_LIBS)

get_legend:  $(MAPSOFT_LIBS)
get_figlib:  $(MAPSOFT_LIBS)
get_text:    $(MAPSOFT_LIBS)
