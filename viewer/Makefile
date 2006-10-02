LDFLAGS = -ggdb `pkg-config --libs gtkmm-2.4,gthread-2.0` -ljpeg -O1
CPPFLAGS = `pkg-config --cflags gtkmm-2.4,gthread-2.0` -I../utils  -O1

all: test

#TEST_LIBS = ../io/io.a ../map/geodata.a ../jeeps/jeeps.a
#TEST_LIBS = ../io_new/io.a ../jeeps/jeeps.a
#TEST_OBJS = test.o ../image-gdk/image.o ../srtm02/srtm.o ../srtm02/point.o 

#test: $(TEST_LIBS) $(TEST_OBJS)
#	$(CXX) $(LDFLAGS) $(TEST_OBJS) $(TEST_LIBS) -o test



clean:
	rm test $(TEST_OBJS)
