#include <gtkmm.h>
#include "gplane.h"
#include "../utils/image_gdk.h"


// define this to reduce information transfer from workplane during scrolling
#define MANUAL_SCROLL

class SimpleViewer : public Gtk::DrawingArea {
  public:

    SimpleViewer(GPlane * pl) :
      plane(pl),
      origin(Point<GCoord>(GCoord_cnt,GCoord_cnt)) {
      set_name("MapsoftViewer");
      add_events ( 
            Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::SCROLL_MASK |
            Gdk::POINTER_MOTION_MASK |
            Gdk::POINTER_MOTION_HINT_MASK );

    }

  void set_origin (const Point<GCoord> & new_origin) {
    Point<int> shift(origin.x>new_origin.x? origin.x-new_origin.x:-(new_origin.x-origin.x),
                     origin.y>new_origin.y? origin.y-new_origin.y:-(new_origin.y-origin.y));
    origin = new_origin;

    get_window()->scroll(shift.x, shift.y);
#ifdef MANUAL_SCROLL
    get_window()->get_update_area();

    if (shift.x >=0){
      draw(Rect<GCoord>(0,0,shift.x,get_height()));
      if (shift.y >=0) draw(Rect<GCoord>(shift.x,0,get_width()-shift.x, shift.y));
      else             draw(Rect<GCoord>(shift.x,get_height()+shift.y, get_width()-shift.x, -shift.y));

    }
    else{
      draw(Rect<GCoord>(get_width() + shift.x, 0, -shift.x, get_height() ));
      if (shift.y >=0) draw(Rect<GCoord>(0,0,get_width()-shift.x, shift.y));
      else             draw(Rect<GCoord>(0,get_height()+shift.y, get_width()-shift.x, -shift.y));
    }
#endif
  }

  Point<GCoord> get_origin (void) {
    return origin;
  }

  void draw (const Rect<GCoord> & r){
    if (r.empty()) return;
    Image<int> img = plane->draw(r + origin);
    Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(img);
    Glib::RefPtr<Gdk::GC> gc = get_style()->get_fg_gc (get_state());
    Glib::RefPtr<Gdk::Window> widget = get_window();
    widget->draw_pixbuf(gc, pixbuf,
          0,0,  r.x, r.y,  r.w, r.h,
          Gdk::RGB_DITHER_NORMAL, 0, 0);
  }


  virtual bool on_expose_event (GdkEventExpose * event){
    Rect<GCoord> r(event->area.x, event->area.y,
                    event->area.width, event->area.height);
    draw(r);
    return true;
  }

  virtual bool on_button_press_event (GdkEventButton * event) {
    if (event->button == 1)
      drag_pos = Point<GCoord> ((GCoord)event->x, (GCoord)event->y);
    return false;
  }

  virtual bool on_motion_notify_event (GdkEventMotion * event) {
    int x=(int)event->x, y=(int)event->y;

    if ((event->state & Gdk::BUTTON1_MASK) &&
        (event->is_hint)){
      set_origin(origin - Point<GCoord>(x,y) + drag_pos);
      drag_pos = Point<GCoord>(x,y);
    }
    return false;
  }

  private:
    GPlane * plane;
    Point<GCoord> origin;
    Point<GCoord> drag_pos;
};

