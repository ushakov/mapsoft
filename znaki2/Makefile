all: make_map_gk update_map get_map get_legend get_print fig2pnm get_figlib make_nom update_ext np2pt

MAPSOFT_LIBS = ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../yaml/yaml.cpp -lsyck ../libfig/libfig.a ../libmp/libmp.a ../utils/libutils.a ../lib2d/lib2d.a

CPPFLAGS=-I../

make_map_gk: $(MAPSOFT_LIBS)
make_nom:    $(MAPSOFT_LIBS)

fig2pnm:     ../utils/pnm_shifter.cpp
pnm_cnt:     ../utils/pnm_shifter.cpp

update_map:  zn_key.cpp zn.cpp $(MAPSOFT_LIBS)
update_ext:  zn_key.cpp zn.cpp $(MAPSOFT_LIBS)
get_map:     zn_key.cpp zn.cpp $(MAPSOFT_LIBS)
get_print:   zn_key.cpp zn.cpp $(MAPSOFT_LIBS)
np2pt:       zn_key.cpp zn.cpp $(MAPSOFT_LIBS)

get_legend:  zn_lists.cpp zn_key.cpp zn.cpp $(MAPSOFT_LIBS)
get_figlib:  zn_lists.cpp zn_key.cpp zn.cpp $(MAPSOFT_LIBS)
get_text:    zn_lists.cpp zn_key.cpp zn.cpp $(MAPSOFT_LIBS)

key_test:    zn.cpp $(MAPSOFT_LIBS)

tmp:         zn_key.cpp $(MAPSOFT_LIBS)


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

