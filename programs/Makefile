LDFLAGS = -ggdb `pkg-config --libs gtkmm-2.4,gthread-2.0` -ljpeg -ltiff -lpng
CPPFLAGS = `pkg-config --cflags gtkmm-2.4,gthread-2.0` -I../utils -I../viewer -I../loaders -I../layers -O1

all: mapsoft_googleview mapsoft_imgview mapsoft_imgview1

#test: ../../mapsoft/io_new/io.a ../jeeps/jeeps.a  ../../mapsoft/image-gdk/image.o

#TEST_LIBS = ../io/io.a ../map/geodata.a ../jeeps/jeeps.a
#TEST_LIBS = ../io_new/io.a ../jeeps/jeeps.a
#TEST_OBJS = test.o ../image-gdk/image.o ../srtm02/srtm.o ../srtm02/point.o 

#test: $(TEST_LIBS) $(TEST_OBJS)
#	$(CXX) $(LDFLAGS) $(TEST_OBJS) $(TEST_LIBS) -o test



clean:
	rm mapsoft_googleview mapsoft_imgview mapsoft_imgview1
