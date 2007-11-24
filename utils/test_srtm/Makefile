# старый способ
#test.fig: test.mp
#	../../vector/mp2fig ../../vector/cnv test.mp test.fig

test.png: test.fig
	fig2dev -D +10:200 -L png -S 2 -m 1.875 test.fig test.png

test.fig: test.mp
	../../znaki/mapconv p test.mp test.fig hr

test.mp: get_hor
	./get_hor 68.01 68.1 38.8 38.99 100 500 ./ > test.mp

get_hor: ../../geo_io/mp.cpp ../line.cpp ../point_int.cpp


CPPFLAGS=-I../../
range_conv: ../../geo_io/libgeo_io.a ../../jeeps/libjeeps.a ../../loaders/libloaders.a -lpng -ljpeg -ltiff ../../utils/libutils.a

