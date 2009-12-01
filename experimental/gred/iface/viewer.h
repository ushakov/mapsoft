#ifndef VIEWER_IFACE_H
#define VIEWER_IFACE_H

#include <gtkmm.h>
#include "../../../core/lib2d/point.h"

/// This is the abstract Viewer to be used with
/// Actions and Rubber objects
class Viewer : public Gtk::DrawingArea {
  public:

    virtual void set_origin (iPoint new_origin) = 0;
    virtual iPoint get_origin (void) const = 0;

    virtual bool is_on_drag() = 0;

    /// These signals are used in Rubber.
    /// They must be called just before and after
    /// actual drawings on Gtk::DrawingArea.
    virtual sigc::signal<void> & signal_before_draw() = 0;
    virtual sigc::signal<void> & signal_after_draw() = 0;
};

#endif
