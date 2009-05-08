
#include <iostream>

#include <gtkmm.h>
#include "gplane.h"
#include "../utils/image_gdk.h"

class SimpleViewer : public Gtk::DrawingArea {
  public:

    SimpleViewer(GPlane * pl) :
      plane(pl),
      origin(Point<GCoord>(GCoord_cnt,GCoord_cnt)) {
      add_events ( 
            Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::SCROLL_MASK |
            Gdk::POINTER_MOTION_MASK |
            Gdk::POINTER_MOTION_HINT_MASK );

    }

  void set_origin (const Point<GCoord> & new_origin) {
    Point<GCoord> shift = origin - new_origin;
    origin -= shift;
    get_window()->scroll(shift.x, 0);
    get_window()->scroll(0, shift.y);
  }

  Point<GCoord> get_origin (void) {
    return origin;
  }


  virtual bool on_expose_event (GdkEventExpose * event){
    Rect<GCoord> r(origin.x + event->area.x, origin.y + event->area.y,
                    event->area.width, event->area.height);

    Image<int> img = plane->draw(r);
    Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(img);
    Glib::RefPtr<Gdk::GC> gc = get_style()->get_fg_gc (get_state());
    Glib::RefPtr<Gdk::Window> widget = get_window();

    widget->draw_pixbuf(gc, pixbuf,
          0,0,
          event->area.x, event->area.y,
          event->area.width, event->area.height,
          Gdk::RGB_DITHER_NORMAL, 0, 0);

    return true;
  }

  virtual bool on_button_press_event (GdkEventButton * event) {
    if (event->button == 1)
      drag_pos = Point<GCoord> ((GCoord)event->x, (GCoord)event->y);

    if (event->button == 2){
      get_window()->scroll(100, 0);
      get_window()->scroll(0, 100);
    }

    return false;
  }

  virtual bool on_motion_notify_event (GdkEventMotion * event) {
    int x=(int)event->x, y=(int)event->y;

    if ((event->state & Gdk::BUTTON1_MASK) &&
        (event->is_hint)){
      set_origin(origin - Point<GCoord>(x,y) + drag_pos);
      drag_pos = Point<GCoord>(x,y);
      // ask for more events
      //  get_pointer(x, y); // ???
    }
    return false;
  }

  private:
    GPlane * plane;
    Point<GCoord> origin;
    Point<GCoord> drag_pos;
};



int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GPlane_test1  pl;

    SimpleViewer viewer(&pl);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
