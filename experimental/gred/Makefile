LDFLAGS= -L../../core/lib2d
LDLIBS= -l2d `pkg-config --libs gtkmm-2.4,gthread-2.0`


CPPFLAGS=`pkg-config --cflags gtkmm-2.4`

TARGETS=simple_viewer_test1\
     simple_viewer_test2\
     mthread_viewer_test1\
     mthread_viewer_test2\
     dthread_viewer_test1\
     dthread_viewer_test2\
     simple_rubb_viewer_test1\
     simple_rubb_viewer_test2\
     mthread_rubb_viewer_test1\
     dthread_rubb_viewer_test1\
     action_viewer_test1

all: $(TARGETS)
clean:
	rm -f -- $(TARGETS)
	rm -f -- *.o

simple_viewer_test1:  gplane.o simple_viewer.o
simple_viewer_test2:  gplane.o simple_viewer.o
mthread_viewer_test1: gplane.o simple_viewer.o mthread_viewer.o
mthread_viewer_test2: gplane.o simple_viewer.o mthread_viewer.o
dthread_viewer_test1: gplane.o simple_viewer.o dthread_viewer.o
dthread_viewer_test2: gplane.o simple_viewer.o dthread_viewer.o
simple_rubb_viewer_test1: gplane.o simple_viewer.o
simple_rubb_viewer_test2: gplane.o simple_viewer.o
mthread_rubb_viewer_test1: gplane.o simple_viewer.o mthread_viewer.o
dthread_rubb_viewer_test1: gplane.o simple_viewer.o dthread_viewer.o
action_viewer_test1: gplane.o simple_viewer.o dthread_viewer.o

gplane.o: gplane.h
dthread_viewer.o: dthread_viewer.h
mthread_viewer.o: mthread_viewer.h
simple_viewer.o: simple_viewer.h
simple_rubb_viewer_test1.o simple_rubb_viewer_test2.o \
  mthread_rubb_viewer_test1.o dthread_rubb_viewer_test1.o: rubber_viewer.h

action_viewer_test1.o: rubber_viewer.h action_viewer.h