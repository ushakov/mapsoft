LDFLAGS= -L../../core/lib2d -L../../core/utils
LDLIBS= -l2d -lutils -lgtest `pkg-config --libs gtkmm-2.4,gthread-2.0`

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
  action_viewer_test2\
  action_viewer_test3\
  viewer_test

TARGETS_MTHREAD=\
  mthread_viewer_test1\
  mthread_viewer_test2\
  mthread_rubb_viewer_test1\
  viewer_test


TARGETS_DTHREAD=\
  dthread_viewer_test1\
  dthread_viewer_test2\
  dthread_rubb_viewer_test1\
  dthread_rubb_viewer_test2\
  action_viewer_test1\
  action_viewer_test2\
  action_viewer_test3\
  viewer_test

RUBBER_OBJECTS=\
  simple_rubb_viewer_test1.o\
  simple_rubb_viewer_test2.o\
  mthread_rubb_viewer_test1.o\
  dthread_rubb_viewer_test1.o\
  action_viewer_test1.o\
  action_viewer_test2.o\
  action_viewer_test3.o\
  viewer_test.o

ACTION_OBJECTS=\
  action_viewer_test1.o\
  action_viewer_test2.o\
  action_viewer_test3.o\
  viewer_test.o

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

simple_viewer_test1: simple_viewer.h gplane.h
simple_viewer_test2: simple_viewer.h gplane.h
mthread_viewer_test1: mthread_viewer.h gplane.h
mthread_viewer_test2: mthread_viewer.h gplane.h

simple_rubb_viewer_test2: simple_rubb_viewer.h gplane.h

tests: supercell_test

supercell_test: supercell_test.cpp supercell.cpp
covering_test: covering_test.cpp covering.cpp supercell.cpp
