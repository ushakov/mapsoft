#ifndef SIMPLE_VIEWER
#define SIMPLE_VIEWER

#include <gtkmm.h>
#include "gobj.h"
#include "viewer.h"

///\addtogroup gred
///@{
///\defgroup simple_viewer
///@{

/**
Простейший GTK-вьюер графического объекта.
Позволяет таскать объект по экрану, перемасштабировать его...
По expose_event необходимые части объекта перерисовываются.
Этот вьюер подходит только для быстро рисующихся объектов!
*/

class SimpleViewer : public Viewer {
  public:

    SimpleViewer(GObj * o = NULL);

    virtual void   set_origin (iPoint new_origin);
    virtual void   set_center (iPoint new_center);
    virtual iPoint get_origin (void) const;
    virtual iPoint get_center (void) const;
    virtual void   set_obj (GObj * o);
    virtual GObj * get_obj (void) const;
    virtual void   set_bgcolor(int c);
    virtual int    get_bgcolor(void) const;
    virtual iRect range() const;

    // draw image from the GObj on the screen
    virtual void draw(const iRect & r);
    // draw the given image on the screen
    virtual void draw_image (const iImage & img, const iPoint & p);
    // draw part of the given image on the screen
    virtual void draw_image (const iImage & img, const iRect & part, const iPoint & p);

    virtual void redraw();
    virtual void rescale(const double k);
    virtual void rescale(const double k, const iPoint & cnt);

    virtual bool on_expose_event (GdkEventExpose * event);
    virtual bool on_button_press_event (GdkEventButton * event);
    virtual bool on_button_release_event (GdkEventButton * event);
    virtual bool on_motion_notify_event (GdkEventMotion * event);

    virtual bool is_on_drag();

    sigc::signal<void> & signal_before_draw();
    sigc::signal<void> & signal_after_draw();
    sigc::signal<void> & signal_draw_error();
    sigc::signal<void> & signal_busy();
    sigc::signal<void> & signal_idle();
    sigc::signal<void, double> & signal_on_rescale();
    sigc::signal<void, iPoint> & signal_ch_origin();

  private:

    sigc::signal<void> signal_before_draw_;
    sigc::signal<void> signal_after_draw_;
    sigc::signal<void> signal_busy_;
    sigc::signal<void> signal_idle_;
    sigc::signal<void, double> signal_on_rescale_;
    sigc::signal<void, iPoint> signal_ch_origin_;

    GObj * obj;
    iPoint origin;

    bool on_drag;
    iPoint drag_pos;

    int bgcolor;
    double sc;
};

#endif
