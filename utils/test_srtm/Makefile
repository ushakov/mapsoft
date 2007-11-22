test.fig: test.mp
	../../vector/mp2fig ../../vector/cnv test.mp test.fig

test.mp: get_hor
	./get_hor 68.0 68.1 38.9 39.0 100 500 > test.mp

get_hor: ../../geo_io/mp.cpp ../line.cpp