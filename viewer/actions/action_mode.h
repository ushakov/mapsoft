#ifndef ACTION_MODE_H
#define ACTION_MODE_H

#include "2d/point.h"
#include <gtkmm.h>

// Interface.
class ActionMode {
public:
    // Returns name of the mode as string.
    virtual std::string get_name() = 0;

    // Activates this mode.
    virtual void activate() = 0;

    // Abandons any action in progress and deactivates mode.
    virtual void abort() = 0;

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) = 0;
};


class ActionModeNone : public ActionMode {
public:
    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "None";
    }

    // Activates this mode.
    virtual void activate() {
    }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) {
    }
};


#endif /* ACTION_MODE_H */
