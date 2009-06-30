#ifndef ACTION_VIEWER_H
#define ACTION_VIEWER_H

#include <gtkmm.h>
#include <sys/time.h> 
#include <vector>
#include <string>

// Interface.
class Action {
public:
  // Returns name of the mode as string.
  virtual std::string get_name() = 0;

  // Activates this mode.
  virtual void init() = 0;

  // Abandons any action in progress and deactivates mode.
  virtual void reset() = 0;

  // Sends user click. Coordinates are in workplane's system.
  virtual void click(iPoint p) = 0;
};

class ActionNone : public Action {
public:
  // Returns name of the mode as string.
  virtual std::string get_name() { return "None"; }

  // Activates this mode.
  virtual void init() { }

  // Abandons any action in progress and deactivates mode.
  virtual void reset() { }

  // Sends user click. Coordinates are in workplane's discrete system.
  virtual void click(iPoint p) { }
};


template <typename ViewerT>
class ActionViewer : public ViewerT {
public:
  ActionViewer(GPlane * pl) : ViewerT(pl){
    current=-1;
    ViewerT::signal_button_press_event().connect (sigc::mem_fun (this, &ActionViewer::on_button_press));
    ViewerT::signal_button_release_event().connect (sigc::mem_fun (this, &ActionViewer::on_button_release));
  }

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

  void action_reset(){
    if (current>=0) actions[current]->reset();
  }

  bool on_button_press (GdkEventButton * event) {
    if (event->button == 1) {
      gettimeofday (&click_started, NULL);
      return true;
    }
    return false;
  }

  bool on_button_release (GdkEventButton * event) {
    if (event->button == 1) {

      struct timeval click_ended;
      gettimeofday (&click_ended, NULL);
      int d = (click_ended.tv_sec - click_started.tv_sec) * 1000 +
              (click_ended.tv_usec - click_started.tv_usec) / 1000; // in ms
      if (d > 250) return true;

      iPoint p(int(event->x), int(event->y));
      p += ViewerT::get_origin();
      if (current>=0) actions[current]->click(p);
      return true;
    }
    return false;
  }

private:
  std::vector<Action *> actions;
  int current;
  struct timeval click_started;
};

#endif



