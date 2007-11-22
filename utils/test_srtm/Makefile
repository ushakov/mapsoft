test.fig: test.mp
	../../vector/mp2fig ../../vector/cnv test.mp test.fig

test.mp: get_hor
	./get_hor 68.40 68.60 38.9 38.99 100 500 > test.mp

get_hor: ../../geo_io/mp.cpp ../line.cpp ../point_int.cpp