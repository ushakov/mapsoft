#include "actions.h"

Actions::Actions(SimpleViewer * v):
    viewer(v),
    current(NULL){

  assert(viewer!=NULL);

  viewer->signal_button_press_event().connect_notify(
    sigc::mem_fun (*this, &Actions::click_begin));
  viewer->signal_button_release_event().connect_notify(
    sigc::mem_fun (*this, &Actions::click_end));
}

/// add action
void
Actions::add(Action * a, std::string name){
  if (name=="") name=a->get_name();
  (*this)[name]=a;
}

/// select current action by name
void
Actions::select (std::string name){
  if (current) current->reset();

  if (count(name)==0){
    current=NULL;
    return;
  }
  current = find(name)->second;
  if (current) current->init();
}


void
Actions::click_begin (GdkEventButton * event) {
  gettimeofday (&click_started, NULL);
  viewer->get_window()->get_pointer(p.x,p.y,state);
  p += viewer->get_origin();
}

void
Actions::click_end (GdkEventButton * event) {
  struct timeval click_ended;
  gettimeofday (&click_ended, NULL);
  int d = (click_ended.tv_sec - click_started.tv_sec) * 1000 +
          (click_ended.tv_usec - click_started.tv_usec) / 1000; // in ms
  if ((d < 250) && current) current->click(p, state);
}
