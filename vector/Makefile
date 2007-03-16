GRLIB_FLAGS = -ljpeg -ltiff -lpng
USB_FLAGS = -lusb
MAPSOFT_LIBS = ../geo_io/io.a ../loaders/loaders.a ../utils/mapsoft_utils.a  ../jeeps/jeeps.a
CPPFLAGS = -O1

all: test_fig test_mp test_geofig mp2fig fig2mp fig_conv

test_fig: fig.o test_fig.cpp
	g++ fig.o test_fig.cpp -o test_fig
test_mp: mp.o test_mp.cpp
	g++ mp.o test_mp.cpp -o test_mp


fig_conv: fig.o fig_conv.cpp
	g++ -g fig.o fig_conv.cpp -o fig_conv


test_geofig: $(MAPSOFT_LIBS) geofig.o fig.o test_geofig.cpp 
	g++ geofig.o fig.o test_geofig.cpp $(GRLIB_FLAGS) $(MAPSOFT_LIBS) -o test_geofig

mp2fig: $(MAPSOFT_LIBS) geofig.o mp.o fig.o mp2fig.cpp 
	g++ -g geofig.o fig.o mp.o mp2fig.cpp $(GRLIB_FLAGS) $(MAPSOFT_LIBS) -o mp2fig
fig2mp: mp2fig
	ln -s mp2fig fig2mp

