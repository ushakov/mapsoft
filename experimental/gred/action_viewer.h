#ifndef ACTION_VIEWER_H
#define ACTION_VIEWER_H

#include <gtkmm.h>
#include <sys/time.h>
#include <vector>
#include <string>
#include "action.h"

template <typename ViewerT>
class ActionViewer : public ViewerT {
public:
  ActionViewer(GObj * pl) : ViewerT(pl), current(-1){}

  void action_add(Action * a){
    actions.push_back(a);
  }

  void action_select (int a) {
    if (current>=0) actions[current]->reset();
    current = a;
    if (current>=0) actions[current]->init();
  }

  void action_clear() {
    actions.clear();
  }

  bool on_button_press_event (GdkEventButton * event) {
    gettimeofday (&click_started, NULL);
    ViewerT::get_window()->get_pointer(p.x,p.y,state);
    p += ViewerT::get_origin();
    return ViewerT::on_button_press_event(event);
  }

  bool on_button_release_event (GdkEventButton * event) {
    struct timeval click_ended;
    gettimeofday (&click_ended, NULL);
    int d = (click_ended.tv_sec - click_started.tv_sec) * 1000 +
            (click_ended.tv_usec - click_started.tv_usec) / 1000; // in ms
    if ((d < 250) && (current>=0)) actions[current]->click(p, state);
    return ViewerT::on_button_release_event(event);
  }

private:
  std::vector<Action *> actions;
  int current;
  struct timeval click_started;
  Gdk::ModifierType state;
  iPoint p;
};

#endif



