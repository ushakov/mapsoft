LDFLAGS= -L../../core/lib2d
LDLIBS= -l2d `pkg-config --libs gtkmm-2.4,gthread-2.0`

CPPFLAGS=`pkg-config --cflags gtkmm-2.4`

TARGETS=\
  simple_viewer_test1\
  simple_viewer_test2\
  mthread_viewer_test1\
  mthread_viewer_test2\
  dthread_viewer_test1\
  dthread_viewer_test2\
  simple_rubb_viewer_test1\
  simple_rubb_viewer_test2\
  mthread_rubb_viewer_test1\
  dthread_rubb_viewer_test1\
  action_viewer_test1\
  action_viewer_test2

TARGETS_MTHREAD=\
  mthread_viewer_test1\
  mthread_viewer_test2\
  mthread_rubb_viewer_test1

TARGETS_DTHREAD=\
  dthread_viewer_test1\
  dthread_viewer_test2\
  dthread_rubb_viewer_test1\
  dthread_rubb_viewer_test2\
  action_viewer_test1\
  action_viewer_test2

RUBBER_OBJECTS=\
  simple_rubb_viewer_test1.o\
  simple_rubb_viewer_test2.o\
  mthread_rubb_viewer_test1.o\
  dthread_rubb_viewer_test1.o\
  action_viewer_test1.o\
  action_viewer_test2.o

ACTION_OBJECTS=\
  action_viewer_test1.o\
  action_viewer_test2.o

all: $(TARGETS)
clean:
	rm -f -- $(TARGETS)
	rm -f -- *.o

$(TARGETS): gplane.o simple_viewer.o
$(TARGETS_MTHREAD): mthread_viewer.o
$(TARGETS_DTHREAD): dthread_viewer.o
$(RUBBER_OBJECTS): rubber_viewer.h
$(ACTION_OBJECTS): action_viewer.h


gplane.cpp: gplane.h
dthread_viewer.cpp: dthread_viewer.h
mthread_viewer.cpp: mthread_viewer.h
simple_viewer.cpp: simple_viewer.h
action_viewer.h: action.h