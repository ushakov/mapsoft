OFILES = mapsoft_time.o mapsoft_convs.o rect.o

all: mapsoft_utils.a 

mapsoft_utils.a: $(OFILES)
	ar rs $@ $(OFILES)

clean:
	rm *.o
