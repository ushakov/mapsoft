#ifndef SIMPLE_VIEWER
#define SIMPLE_VIEWER

#include <gtkmm.h>
#include "iface/gobj.h"

class SimpleViewer : public Gtk::DrawingArea {
  public:

    SimpleViewer(GObj * pl);

    virtual void   set_origin (iPoint new_origin);
    virtual iPoint get_origin (void) const;
    virtual void   set_obj (GObj * o);
    virtual GObj * get_obj (void) const;

    virtual void draw(const iRect & r);
    virtual void draw_image (const iImage & img, const iPoint & p);
    virtual void draw_image (const iImage & img, const iRect & part, const iPoint & p);

    virtual void redraw();

    virtual bool on_expose_event (GdkEventExpose * event);
    virtual bool on_button_press_event (GdkEventButton * event);
    virtual bool on_button_release_event (GdkEventButton * event);
    virtual bool on_motion_notify_event (GdkEventMotion * event);

    virtual bool is_on_drag();
    virtual int  get_epoch();
    virtual void inc_epoch();

    sigc::signal<void> signal_before_draw;
    sigc::signal<void> signal_after_draw;

  private:

    GObj * obj;
    iPoint origin;

    bool on_drag;
    iPoint drag_pos;

    int epoch;
};

#endif
