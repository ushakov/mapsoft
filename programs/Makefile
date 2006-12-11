GTTMM_FLAGS = `pkg-config --libs gtkmm-2.4,gthread-2.0` 
GRLIB_FLAGS = -ljpeg -ltiff -lpng 
GRAPH_FLAGS = $(GTTMM_FLAGS) $(GRLIB_FLAGS)
GOOGLE_FLAGS = `pkg-config --libs libcurl`
USB_FLAGS = -lusb


CPPFLAGS = `pkg-config --cflags libcurl` `pkg-config --cflags gtkmm-2.4,gthread-2.0` -I../utils -I../viewer -I../loaders -I../layers -O1

#all: mapsoft_googleview mapsoft_imgview mapsoft_imgview1 mapsoft_convert

#test: ../../mapsoft/io_new/io.a ../jeeps/jeeps.a  ../../mapsoft/image-gdk/image.o

#TEST_LIBS = ../io/io.a ../map/geodata.a ../jeeps/jeeps.a
#TEST_LIBS = ../io_new/io.a ../jeeps/jeeps.a
#TEST_OBJS = test.o ../image-gdk/image.o ../srtm02/srtm.o ../srtm02/point.o 

#test: $(TEST_LIBS) $(TEST_OBJS)
#	$(CXX) $(LDFLAGS) $(TEST_OBJS) $(TEST_LIBS) -o test

all: mapsoft_convert mapsoft_googleview1 mapsoft_imgview mapsoft_imgview1 mapsoft_mapview

clean:
	rm mapsoft_googleview mapsoft_imgview mapsoft_imgview1 mapsoft_convert *.o

MAPSOFT_LIBS = ../geo_io/io.a ../loaders/loaders.a ../utils/mapsoft_utils.a ../jeeps/jeeps.a 

alllibs.a: $(MAPSOFT_LIBS)
	ar rs alllibs.a $(MAPSOFT_LIBS)


mapsoft_convert: $(MAPSOFT_LIBS) mapsoft_convert.o alllibs.a
	g++ mapsoft_convert.o alllibs.a $(GRLIB_FLAGS) $(LDFLAGS)  -o $@ 

mapsoft_mkmap: $(MAPSOFT_LIBS) mapsoft_mkmap.o
	g++ mapsoft_mkmap.o $(MAPSOFT_LIBS) $(GRLIB_FLAGS) $(LDFLAGS)  -o $@ 

mapsoft_googleview1: $(MAPSOFT_LIBS) mapsoft_googleview1.o
	g++ mapsoft_googleview1.o $(MAPSOFT_LIBS) $(GRAPH_FLAGS) $(USB_FLAGS) $(GOOGLE_FLAGS) -o $@ 

mapsoft_mapview: $(MAPSOFT_LIBS) mapsoft_mapview.o
	g++ mapsoft_mapview.o $(MAPSOFT_LIBS)  $(GRAPH_FLAGS) $(USB_FLAGS) -o $@ 

mapsoft_imgview: $(MAPSOFT_LIBS) mapsoft_imgview.o
	g++ mapsoft_imgview.o $(MAPSOFT_LIBS) $(GRAPH_FLAGS) $(USB_FLAGS) -o $@ 

mapsoft_imgview1: $(MAPSOFT_LIBS) mapsoft_imgview1.o
	g++ mapsoft_imgview1.o $(MAPSOFT_LIBS) $(GRAPH_FLAGS) $(USB_FLAGS) -o $@ 

mapsoft_srtmview: $(MAPSOFT_LIBS) mapsoft_srtmview.o
	g++ mapsoft_srtmview.o $(MAPSOFT_LIBS) $(GRAPH_FLAGS) $(USB_FLAGS) -o $@ 
