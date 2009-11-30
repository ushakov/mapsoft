#ifndef ACTION_VIEWER_H
#define ACTION_VIEWER_H

#include <sys/time.h>
#include <map>
#include <string>
#include "iface/action.h"
#include "simple_viewer.h"
#include "../../core/lib2d/point.h"

class Actions : public std::map<std::string, Action *> {
public:

  Actions(SimpleViewer * v);

  /// add action;/ if name=="" try to get it from action->get_name()
  void add(Action * a, std::string name="");

  /// select current action by name
  void select(std::string name);

private:

  /// helper functions to be connected with viewer signals
  void click_begin (GdkEventButton * event);
  void click_end (GdkEventButton * event);

  Action * current;
  struct timeval click_started;
  Gdk::ModifierType state;
  iPoint p;
  SimpleViewer * viewer;
};

#endif
