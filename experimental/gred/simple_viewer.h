#ifndef SIMPLE_VIEWER
#define SIMPLE_VIEWER

#include <gtkmm.h>
#include "gplane.h"

class SimpleViewer : public Gtk::DrawingArea {
  public:

  SimpleViewer(GPlane * pl);

  virtual void set_origin (const Point<int> & new_origin);
  Point<int> get_origin (void) const;

  virtual void draw(const Rect<int> & r);
  virtual void draw_image (const Image<int> & img, const Point<int> & p);

  virtual bool on_expose_event (GdkEventExpose * event);
  virtual bool on_button_press_event (GdkEventButton * event);
  virtual bool on_button_release_event (GdkEventButton * event);
  virtual bool on_motion_notify_event (GdkEventMotion * event);

  GPlane * plane;
  Point<int> origin;
  Point<int> drag_pos;
  bool on_drag;
};

#endif
