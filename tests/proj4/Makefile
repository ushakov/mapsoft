
all: t1 t1a t2

t1: t1.c
	cc $< -o $@ -lproj

t2: t2.c
	cc $< -o $@ -lproj

t3: t3.c
	cc $< -o $@ -lproj

t1a: t1a.cpp
	g++  $< -o $@\
		-I../../core/ -L../../core/jeeps -L../../core/libgeo -L../../core/lib2d\
		-L../../core/loaders -L../../core/utils\
                -lgeo -ljeeps -lloaders -lutils -l2d -lpng -ljpeg -ltiff -lproj

t3a: t3a.cpp
	g++  $< -o $@\
		-I../core/ -L../core/jeeps -L../core/libgeo -L../core/lib2d\
		-L../core/loaders -L../core/utils\
                -lgeo -ljeeps -lloaders -lutils -l2d -lpng -ljpeg -ltiff