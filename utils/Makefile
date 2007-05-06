OFILES = mapsoft_time.o mapsoft_options.o rect.o

CXXFLAGS=-ggdb
LDFLAGS=-ggdb

all: mapsoft_utils.a 

generic_accessor_unittest: generic_accessor_unittest.o
	$(CXX) $(LDFLAGS) $^ -o $@

mapsoft_utils.a: $(OFILES)
	ar rs $@ $(OFILES)

clean:
	rm *.o
