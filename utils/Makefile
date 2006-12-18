OFILES = mapsoft_geo.o mapsoft_options.o mapsoft_time.o mapsoft_convs.o

all: mapsoft_utils.a rect.o

mapsoft_utils.a: $(OFILES)
	ar rs $@ $(OFILES)


clean:
	rm *.o
