GTTMM_FLAGS = `pkg-config --libs gtkmm-2.4,gthread-2.0` 
GRLIB_FLAGS = -ljpeg -ltiff -lpng 
GRAPH_FLAGS = $(GTTMM_FLAGS) $(GRLIB_FLAGS)
GOOGLE_FLAGS = `pkg-config --libs libcurl`
USB_FLAGS = -lusb


CPPFLAGS = `pkg-config --cflags libcurl` `pkg-config --cflags gtkmm-2.4,gthread-2.0`\
     -I../utils -I../viewer -I../loaders -I../layers -O1

#all: mapsoft_googleview mapsoft_imgview mapsoft_imgview1 mapsoft_convert

#test: ../../mapsoft/io_new/io.a ../jeeps/jeeps.a  ../../mapsoft/image-gdk/image.o

#TEST_LIBS = ../io/io.a ../map/geodata.a ../jeeps/jeeps.a
#TEST_LIBS = ../io_new/io.a ../jeeps/jeeps.a
#TEST_OBJS = test.o ../image-gdk/image.o ../srtm02/srtm.o ../srtm02/point.o 

#test: $(TEST_LIBS) $(TEST_OBJS)
#	$(CXX) $(LDFLAGS) $(TEST_OBJS) $(TEST_LIBS) -o test

all: mapsoft_convert mapsoft_googleview1 mapsoft_imgview mapsoft_imgview1 mapsoft_mapview mapsoft_mkmap mapsoft_mapcnv mapsoft_ref

clean:
	rm mapsoft_googleview mapsoft_imgview mapsoft_imgview1 mapsoft_convert *.o

MAPSOFT_LIBS = ../geo_io/io.a ../loaders/loaders.a ../utils/mapsoft_utils.a  ../jeeps/jeeps.a 

mapsoft_convert: $(MAPSOFT_LIBS) mapsoft_convert.cpp
	g++ mapsoft_convert.cpp $(CPPFLAGS) $(MAPSOFT_LIBS) $(USB_FLAGS) $(GRLIB_FLAGS) $(LDFLAGS)  -o $@ 

mapsoft_ref: $(MAPSOFT_LIBS) mapsoft_ref.cpp
	g++ mapsoft_ref.cpp $(CPPFLAGS) $(MAPSOFT_LIBS) $(USB_FLAGS) $(GRLIB_FLAGS) $(LDFLAGS)  -o $@ 

mapsoft_mkmap: $(MAPSOFT_LIBS) mapsoft_mkmap.cpp
	g++ mapsoft_mkmap.cpp $(CPPFLAGS) $(MAPSOFT_LIBS) $(USB_FLAGS) $(GRLIB_FLAGS) $(LDFLAGS)  -o $@ 

mapsoft_googleview1: $(MAPSOFT_LIBS) mapsoft_googleview1.cpp
	g++  mapsoft_googleview1.cpp $(CPPFLAGS) $(MAPSOFT_LIBS) $(GRAPH_FLAGS) $(USB_FLAGS) $(GOOGLE_FLAGS) -o $@ 

mapsoft_mapview: $(MAPSOFT_LIBS) mapsoft_mapview.cpp ../layers/layer_geomap.h ../layers/layer_geodata.h
	g++  mapsoft_mapview.cpp  $(CPPFLAGS) $(MAPSOFT_LIBS) $(GRAPH_FLAGS) $(USB_FLAGS) -o $@ 

mapsoft_mapcnv: $(MAPSOFT_LIBS) mapsoft_mapcnv.cpp ../layers/layer_geomap.h
	g++  mapsoft_mapcnv.cpp  $(CPPFLAGS) $(MAPSOFT_LIBS) $(GRAPH_FLAGS) $(USB_FLAGS) -o $@ 

mapsoft_imgview: $(MAPSOFT_LIBS) mapsoft_imgview.o
	g++ mapsoft_imgview.o $(CPPFLAGS) $(MAPSOFT_LIBS) $(GRAPH_FLAGS) $(USB_FLAGS) -o $@ 

mapsoft_imgview1: $(MAPSOFT_LIBS) mapsoft_imgview1.o
	g++ mapsoft_imgview1.o $(CPPFLAGS) $(MAPSOFT_LIBS) $(GRAPH_FLAGS) $(USB_FLAGS) -o $@ 

mapsoft_srtmview: $(MAPSOFT_LIBS) mapsoft_srtmview.o
	g++ mapsoft_srtmview.o $(CPPFLAGS) $(MAPSOFT_LIBS) $(GRAPH_FLAGS) $(USB_FLAGS) -o $@ 
