all: make_map_gk update_map get_map get_legend

CPPFLAGS=-I../
make_map_gk: ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a
update_map:  ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck zn.cpp 
get_map:     ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck zn.cpp 
get_legend: zn.cpp ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck
get_text:   zn.cpp ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck
get_print:  zn.cpp ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a ../yaml/yaml.cpp -lsyck

key_test: zn.cpp ../geo_io/libgeo_io.a ../jeeps/libjeeps.a ../loaders/libloaders.a -lpng -ljpeg -ltiff ../utils/libutils.a



l: mmb.png hr.png mmb.conv.png hr.conv.png

FIGCNV = ../vector/fig_conv_podm
FIGCYR = ~/bin/figcyr

%.conv.fig: %.fig
	$(FIGCNV) $< $@
%.eps: %.fig
	fig2dev  -L eps -m 3.75 -F  $< | $(FIGCYR) > $@
%.png: %.eps
	gs -q -dNOPAUSE -dBATCH -dSAFER -dEPSCrop -sDEVICE=png16m -r80 -sOutputFile=$@ $<


mmb.fig: mmb.cnf get_legend
	./get_legend mmb.cnf > mmb.fig
hr.fig: hr.cnf get_legend
	./get_legend hr.cnf > hr.fig

