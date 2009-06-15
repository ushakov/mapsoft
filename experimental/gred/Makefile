LDFLAGS= -L../../core/lib2d
LDLIBS= -l2d -lgtest `pkg-config --libs gtkmm-2.4,gthread-2.0`


CPPFLAGS=`pkg-config --cflags gtkmm-2.4`

all: simple_viewer_test1 simple_viewer_test2\
      mthread_viewer_test1 mthread_viewer_test2\
      simple_rubb_viewer_test2

simple_viewer_test1: simple_viewer.h gplane.h
simple_viewer_test2: simple_viewer.h gplane.h
mthread_viewer_test1: mthread_viewer.h gplane.h
mthread_viewer_test2: mthread_viewer.h gplane.h

tests: supercell_test

supercell_test: supercell_test.cpp supercell.cpp
simple_rubb_viewer_test2: simple_rubb_viewer.h gplane.h
