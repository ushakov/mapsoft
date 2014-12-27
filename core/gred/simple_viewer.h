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
    virtual iPoint get_origin (void) const { return origin;}

    virtual void   set_center (iPoint new_center){
      set_origin(new_center - iPoint(get_width(), get_height())/2);}
    virtual iPoint get_center (void) const {
      return origin + iPoint(get_width(), get_height())/2;}

    virtual void   set_obj (GObj * o) {obj=o; redraw();}
    virtual GObj * get_obj (void) const {return obj;}

    virtual void   set_bgcolor(int c) {bgcolor=c | 0xFF000000;}
    virtual int    get_bgcolor(void) const {return bgcolor;}

    virtual iRect range() const {
      return obj?obj->range():GOBJ_MAX_RANGE;}

    // draw image from the GObj on the screen
    virtual void draw(const iRect & r);
    // draw the given image on the screen
    virtual void draw_image (const iImage & img, const iPoint & p);
    // draw part of the given image on the screen
    virtual void draw_image (const iImage & img, const iRect & part, const iPoint & p);

    virtual void redraw();
    void start_waiting(){ waiting++;}
    void stop_waiting(){ waiting--; if (waiting==0) redraw();}
    bool is_waiting() const { return waiting!=0; }

    virtual void rescale(const double k);
    virtual void rescale(const double k, const iPoint & cnt);

    virtual bool on_expose_event (GdkEventExpose * event);
    virtual bool on_button_press_event (GdkEventButton * event);
    virtual bool on_button_release_event (GdkEventButton * event);
    virtual bool on_motion_notify_event (GdkEventMotion * event);
    virtual bool on_key_press (GdkEventKey * event); // read note in simple_viewer.cpp
    virtual bool on_scroll_event (GdkEventScroll * event);

    virtual bool is_on_drag() const {return on_drag;}

    sigc::signal<void> & signal_before_draw() {return signal_before_draw_;}
    sigc::signal<void> & signal_after_draw()  {return signal_after_draw_;}
    sigc::signal<void> & signal_busy()        {return signal_busy_;}
    sigc::signal<void> & signal_idle()        {return signal_idle_;}
    sigc::signal<void, double> & signal_on_rescale()  {return signal_on_rescale_;}
    sigc::signal<void, iPoint> & signal_ch_origin()   {return signal_ch_origin_;}
    sigc::signal<void, iPoint, int, const Gdk::ModifierType&> & signal_click() {return signal_click_;}

    GObj * get_gobj() {return obj;}

  private:

    sigc::signal<void> signal_before_draw_;
    sigc::signal<void> signal_after_draw_;
    sigc::signal<void> signal_busy_;
    sigc::signal<void> signal_idle_;
    sigc::signal<void, double> signal_on_rescale_;
    sigc::signal<void, iPoint> signal_ch_origin_;
    sigc::signal<void, iPoint, int, const Gdk::ModifierType&> signal_click_;

    GObj * obj;
    iPoint origin;

    bool on_drag;
    iPoint drag_pos, drag_start;
    unsigned int waiting;

    int bgcolor;
    double sc;
};

#endif
