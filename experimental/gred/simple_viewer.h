#include <gtkmm.h>
#include <gdk/gdk.h>

#include "gplane.h"
#include "../../core/utils/image_gdk.h"

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
  }

  Point<GCoord> get_origin (void) {
    return origin;
  }

  virtual void draw(const Rect<GCoord> & r){
    if (r.empty()) return;
    draw_image(plane->draw(r + origin), r.TLC());
  }

  virtual void draw_image (const Image<int> & img, const Point<int> & p){
    Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(img);
    Glib::RefPtr<Gdk::GC> gc = get_style()->get_fg_gc (get_state());
    Glib::RefPtr<Gdk::Window> widget = get_window();
    widget->draw_pixbuf(gc, pixbuf,
          0,0,  p.x, p.y,  img.w, img.h,
          Gdk::RGB_DITHER_NORMAL, 0, 0);
  }

  virtual bool on_expose_event (GdkEventExpose * event){
    GdkRectangle *rects;
    int nrects;
    gdk_region_get_rectangles(event->region, &rects, &nrects);
    for (int i = 0; i < nrects; ++i) {
      Rect<GCoord> r(rects[i].x, rects[i].y,
		     rects[i].width, rects[i].height);
      draw(r);
    }
    g_free(rects);
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

  GPlane * plane;
  Point<GCoord> origin;
  Point<GCoord> drag_pos;
};

