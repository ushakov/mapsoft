LDFLAGS= -L../../core/lib2d
LDLIBS= -l2d `pkg-config --libs gtkmm-2.4,gthread-2.0`


CPPFLAGS=`pkg-config --cflags gtkmm-2.4`

all: simple_viewer_test1 simple_viewer_test2\
      mthread_viewer_test1 mthread_viewer_test2\
      simple_rubb_viewer_test2

simple_viewer_test1:  gplane.o simple_viewer.o
simple_viewer_test2:  gplane.o simple_viewer.o
mthread_viewer_test1: gplane.o simple_viewer.o
mthread_viewer_test2: gplane.o simple_viewer.o

simple_rubb_viewer_test2:
