GRAPH_FLAGS = `pkg-config --libs gtkmm-2.4,gthread-2.0` -ljpeg -ltiff -lpng 
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

all: mapsoft_convert mapsoft_googleview1

clean:
	rm mapsoft_googleview mapsoft_imgview mapsoft_imgview1 mapsoft_convert *.o

MAPSOFT_LIBS = ../geo_io/io.a ../utils/mapsoft_utils.a ../jeeps/jeeps.a -lusb


mapsoft_convert: $(MAPSOFT_LIBS) mapsoft_convert.o
	g++ mapsoft_convert.o $(MAPSOFT_LIBS) $(LDFLAGS)  -o $@ 

mapsoft_googleview1: $(MAPSOFT_LIBS) mapsoft_googleview1.o
	g++ mapsoft_googleview1.o $(MAPSOFT_LIBS) $(GRAPH_FLAGS) $(USB_FLAGS) $(GOOGLE_FLAGS) -o $@ 
