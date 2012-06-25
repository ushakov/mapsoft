GTTMM_FLAGS = `pkg-config --libs gtkmm-2.4,gthread-2.0`
GRLIB_FLAGS = -ljpeg -ltiff -lpng
GRAPH_FLAGS = $(GTTMM_FLAGS) $(GRLIB_FLAGS)
GOOGLE_FLAGS = `pkg-config --libs libcurl`
USB_FLAGS = -lusb

CPPFLAGS = `pkg-config --cflags libcurl` `pkg-config --cflags gtkmm-2.4,gthread-2.0`\
     -I../utils -I../viewer -I../loaders -I../layers -O1
MAPSOFT_LIBS = ../rect.o ../../geo_io/io.a ../../utils/mapsoft_utils.a  ../../loaders/loaders.a ../../jeeps/jeeps.a


all: map2pt
#all: image2pixbuf options_test convs_test

geo_convs_test: ../../jeeps/jeeps.a


convs_test: ../../jeeps/jeeps.a ../mapsoft_utils.a


map2pt: $(MAPSOFT_LIBS) map2pt.o
	g++ map2pt.o $(MAPSOFT_LIBS) $(USB_FLAGS) $(GRLIB_FLAGS) $(LDFLAGS)  -o $@
