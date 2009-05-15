# старый способ
#test.fig: test.mp
#	../../vector/mp2fig ../../vector/cnv test.mp test.fig

RANGE = 69 430000 440000 4300000 4310000

test.png: test.fig
	fig2dev -D +10:200 -L png -S 2 -m 1.875 test.fig test.png

test.fig: test.mp mk_gk_fig
	./mk_gk_fig 1000 $(RANGE) > test.fig
	../../znaki/mapconv p test.mp test.fig hr

test.mp: get_hor_gk
	./get_hor_gk $(RANGE) 100 500 ./ > test.mp

#test.mp: get_hor
#	./get_hor 68.01 68.1 38.8 38.99 100 500 ./ > test.mp

get_hor: ../../libmp/libmp.a ../../utils/libutils.a 

CPPFLAGS=-I../../
range_conv: ../../geo_io/libgeo_io.a ../../jeeps/libjeeps.a ../../loaders/libloaders.a -lpng -ljpeg -ltiff ../../utils/libutils.a
get_hor_gk: ../../geo_io/libgeo_io.a ../../jeeps/libjeeps.a ../../loaders/libloaders.a -lpng -ljpeg -ltiff ../../utils/libutils.a
mk_gk_fig: ../../geo_io/libgeo_io.a ../../jeeps/libjeeps.a ../../loaders/libloaders.a -lpng -ljpeg -ltiff ../../utils/libutils.a

