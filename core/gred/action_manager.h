#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include <sys/time.h>
#include <map>
#include <string>
#include "action.h"
#include "viewer.h"
#include "2d/point.h"

class ActionManager : public std::map<std::string, Action *> {
public:

  ActionManager(Viewer * v);

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
  Viewer * viewer;
};

#endif
