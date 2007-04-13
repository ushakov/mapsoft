GRLIB_FLAGS = -ljpeg -ltiff -lpng
USB_FLAGS = -lusb
MAPSOFT_LIBS = ../geo_io/io.a ../loaders/loaders.a ../utils/mapsoft_utils.a  ../jeeps/jeeps.a
CPPFLAGS = -O1


all:  test_geofig mp2fig fig2mp fig_conv fig_norm

test_fig: $(MAPSOFT_LIBS) test_fig.cpp
	g++ test_fig.cpp $(MAPSOFT_LIBS) -o test_fig
test_mp: $(MAPSOFT_LIBS) test_mp.cpp
	g++ test_mp.cpp $(MAPSOFT_LIBS) -o test_mp


fig_conv: $(MAPSOFT_LIBS) fig_conv.cpp
	g++ fig_conv.cpp $(MAPSOFT_LIBS) -o fig_conv
fig_ukl: $(MAPSOFT_LIBS) fig_ukl.cpp
	g++ fig_ukl.cpp $(MAPSOFT_LIBS) -o fig_ukl

fig_norm: $(MAPSOFT_LIBS) fig_norm.cpp
	g++ fig_norm.cpp $(MAPSOFT_LIBS) -o fig_norm


test_geofig: $(MAPSOFT_LIBS) test_geofig.cpp 
	g++ test_geofig.cpp $(GRLIB_FLAGS) $(MAPSOFT_LIBS) -o test_geofig

mp2fig: $(MAPSOFT_LIBS) mp2fig.cpp 
	g++ mp2fig.cpp $(GRLIB_FLAGS) $(MAPSOFT_LIBS) -o mp2fig
fig2mp: mp2fig
	ln -s mp2fig fig2mp

