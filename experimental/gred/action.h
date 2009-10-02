#ifndef ACTION_H
#define ACTION_H

#include <gtkmm.h>
#include <string>

// Action interface
class Action {
public:
  // Returns name of the mode as string.
  virtual std::string get_name() = 0;

  // Activates this mode.
  virtual void init() = 0;

  // Abandons any action in progress and deactivates mode.
  virtual void reset() = 0;

  // Sends user click. Coordinates are in workplane's system.
  virtual void click(const iPoint & p, const Gdk::ModifierType & state) = 0;
};
#endif



