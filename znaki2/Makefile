all: make_map_gk update_map get_map get_legend get_print fig2pnm get_figlib make_nom

CPPFLAGS=-I../
make_map_gk: ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a
update_map:  ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck zn_key.cpp zn.cpp 
get_map:     ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck zn_key.cpp zn.cpp 
get_print:   zn_key.cpp zn.cpp ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck

get_legend: zn_lists.cpp zn_key.cpp zn.cpp ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck
get_figlib: zn_lists.cpp zn_key.cpp zn.cpp ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck
get_text:   zn_lists.cpp zn_key.cpp zn.cpp ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck

key_test: zn.cpp ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a

make_nom:  ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a


tmp: zn_key.cpp ../geo_io/fig.cpp ../utils/libutils.a



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

