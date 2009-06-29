#ifndef ACTION_VIEWER_H
#define ACTION_VIEWER_H

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
  ActionViewer(GPlane * pl) : ViewerT(pl){}

  void add_action(Action * a){
    actions.push_back(a);
  }

  void select_action (int a) {
    actions[current]->reset();
    current = a;
    actions[current]->init();
  }

  void clear() {
    actions.clear();
  }

  void reset(){
    actions[current]->reset();
  }

  void click (iPoint p) {
    actions[current]->click(p);
  }

private:
  std::vector<Action *> actions;
  int current;
};

#endif



