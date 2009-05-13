#include <gtkmm.h>

#include <queue>
#include <map>

#include "gplane.h"
#include "../utils/image_gdk.h"


// define this to reduce information transfer from workplane during scrolling
#define MANUAL_SCROLL


class ThreadViewer : public Gtk::DrawingArea {
  public:

    ThreadViewer(GPlane * pl1, GPlane * pl2) :
        plane1(pl1),
        plane2(pl2),
        origin(Point<GCoord>(GCoord_cnt,GCoord_cnt)) {

      Glib::thread_init();
      done_signal.connect(sigc::mem_fun(*this, &ThreadViewer::on_done_signal));

      mutex = new(Glib::Mutex);
      set_name("MapsoftViewer");
      add_events ( 
            Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::SCROLL_MASK |
            Gdk::POINTER_MOTION_MASK |
            Gdk::POINTER_MOTION_HINT_MASK );

    }
    ~ThreadViewer(){
      delete(mutex);
    }



  void set_origin (const Point<GCoord> & new_origin) {
    Point<int> shift=get_shift(origin, new_origin);
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
      if (shift.y >=0) draw(Rect<GCoord>(0,0,get_width()+shift.x, shift.y));
      else             draw(Rect<GCoord>(0,get_height()+shift.y, get_width()+shift.x, -shift.y));
    }
#endif
  }

  Point<GCoord> get_origin (void) {
    return origin;
  }

  void updater(const Rect<GCoord> & r){
    std::pair<Point<GCoord>, Image<int> > p(r.TLC(), plane2->draw(r));

    mutex->lock();
    done_cache.insert(p);
    mutex->unlock();

    done_signal.emit();
  }

  void on_done_signal(){
    mutex->lock();
    for (std::map<Point<GCoord>,Image<int> >::const_iterator 
         i=done_cache.begin(); i!=done_cache.end(); i++){

      draw_image(i->second, get_shift(i->first, origin));
    }
    done_cache.clear();
    mutex->unlock();
  }

  void draw(const Rect<GCoord> & r){
    if (r.empty()) return;
    draw_image(plane1->draw(r + origin), r.TLC());

    Glib::Thread * tile_updater_thread =
      Glib::Thread::create(
        sigc::bind<1>(sigc::mem_fun(*this, &ThreadViewer::updater),
          r + origin),
        false);
  }

  void draw_image (const Image<int> & img, const Point<int> & p){
    Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(img);
    Glib::RefPtr<Gdk::GC> gc = get_style()->get_fg_gc (get_state());
    Glib::RefPtr<Gdk::Window> widget = get_window();
    widget->draw_pixbuf(gc, pixbuf,
          0,0,  p.x, p.y,  img.w, img.h,
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
    GPlane *plane1, *plane2;
    Point<GCoord> origin;
    Point<GCoord> drag_pos;

    std::map<Point<GCoord>,Image<int> > done_cache;
    Glib::Dispatcher          done_signal;
    Glib::Mutex              *mutex;

};

