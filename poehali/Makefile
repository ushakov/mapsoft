CFLAGS = `pkg-config --cflags gdk-pixbuf-2.0 gdk-2.0`
LDFLAGS = `pkg-config --libs gdk-pixbuf-2.0 gdk-2.0`

all: poecode

poecode: poecode.o
	gcc -o $@ $< $(LDFLAGS)

poecode.o: poecode.c
	gcc -c -o $@ $(CFLAGS) $<
