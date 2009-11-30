#ifndef ACTION_VIEWER_H
#define ACTION_VIEWER_H

#include <sys/time.h>
#include <map>
#include <string>
#include "action.h"
#include "simple_viewer.h"

class Actions {
public:

  Actions(SimpleViewer * v):
      viewer(v),
      current(NULL){
    assert(viewer!=NULL);
    viewer->signal_button_press_event().connect_notify(
      sigc::mem_fun (*this, &Actions::click_begin));
    viewer->signal_button_release_event().connect_notify(
      sigc::mem_fun (*this, &Actions::click_end));
  }

  void add(Action * a, std::string name=""){
    if (name=="") name=a->get_name();
    actions.insert(
      std::pair<std::string, Action *>(name, a));
  }

  void select (std::string name){
    if (current) current->reset();

    if (actions.count(name)==0){
      current=NULL;
      return;
    }
    current = actions.find(name)->second;
    if (current) current->init();
  }

  void clear() {
    actions.clear();
  }

private:

  void click_begin (GdkEventButton * event) {
    gettimeofday (&click_started, NULL);
    viewer->get_window()->get_pointer(p.x,p.y,state);
    p += viewer->get_origin();
  }

  void click_end (GdkEventButton * event) {
    struct timeval click_ended;
    gettimeofday (&click_ended, NULL);
    int d = (click_ended.tv_sec - click_started.tv_sec) * 1000 +
            (click_ended.tv_usec - click_started.tv_usec) / 1000; // in ms
    if ((d < 250) && current) current->click(p, state);
  }

  std::map<std::string, Action *> actions;
  Action * current;
  struct timeval click_started;
  Gdk::ModifierType state;
  iPoint p;
  SimpleViewer * viewer;
};

#endif



