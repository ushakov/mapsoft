all: make_map_gk get_legend get_print fig2pnm get_figlib make_nom np2pt tx2pt

MAPSOFT_LIBS = ../libzn/libzn.a ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff -lusb ../yaml/yaml.cpp -lsyck ../libfig/libfig.a ../libmp/libmp.a ../utils/libutils.a ../lib2d/lib2d.a

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

l: mmb.png hr.png mmb.conv.png hr.conv.png mmb_lib hr_lib

%.conv.fig: %.fig ./get_print
	./get_print `basename $< .fig`.cnf $< $@
%.png: %.fig
	./fig2pnm $< | pnmtopng > $@
%.png: %.eps
	gs -q -dNOPAUSE -dBATCH -dSAFER -dEPSCrop -sDEVICE=png16m -r80 -sOutputFile=$@ $<


mmb.fig: mmb.cnf get_legend
	./get_legend mmb.cnf > mmb.fig
hr.fig: hr.cnf get_legend
	./get_legend hr.cnf > hr.fig

mmb_lib: mmb.cnf get_figlib
	if [ ! -d lib_mmb ]; then mkdir fig_mmb; fi
	./get_figlib mmb.cnf fig_mmb

hr_lib: hr.cnf get_figlib
	if [ ! -d lib_hr ]; then mkdir fig_hr; fi
	./get_figlib hr.cnf fig_hr

