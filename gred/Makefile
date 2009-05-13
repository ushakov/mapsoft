LDFLAGS= -L../lib2d
LDLIBS= -l2d `pkg-config --libs gtkmm-2.4,gthread-2.0`


CPPFLAGS=-I../ `pkg-config --cflags gtkmm-2.4`

all: simple_viewer_test mthread_viewer_test

simple_viewer_test: simple_viewer.h gplane.h
mthread_viewer_test: mthread_viewer.h gplane.h
