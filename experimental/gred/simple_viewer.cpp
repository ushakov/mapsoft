#include "simple_viewer.h"

#include <gdk/gdk.h>
#include "../../core/utils/image_gdk.h"

SimpleViewer::SimpleViewer(GPlane * pl) :
    plane(pl),
    origin(Point<int>(0,0)),
    on_drag(false) {
  set_name("MapsoftViewer");
  add_events (
    Gdk::BUTTON_PRESS_MASK |
    Gdk::BUTTON_RELEASE_MASK |
    Gdk::SCROLL_MASK |
    Gdk::POINTER_MOTION_MASK |
    Gdk::POINTER_MOTION_HINT_MASK );
}

void SimpleViewer::set_origin (const Point<int> & new_origin) {
  get_window()->scroll(origin.x-new_origin.x, origin.y-new_origin.y);
  origin = new_origin;
}

Point<int> SimpleViewer::get_origin (void) const {
  return origin;
}

void SimpleViewer::draw(const Rect<int> & r){
  if (r.empty()) return;
  draw_image(plane->draw(r + origin), r.TLC());
}

void SimpleViewer::draw_image (const Image<int> & img, const Point<int> & p){
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(img);
  Glib::RefPtr<Gdk::GC> gc = get_style()->get_fg_gc (get_state());
  Glib::RefPtr<Gdk::Window> widget = get_window();
  widget->draw_pixbuf(gc, pixbuf,
          0,0,  p.x, p.y,  img.w, img.h,
          Gdk::RGB_DITHER_NORMAL, 0, 0);
}

bool SimpleViewer::on_expose_event (GdkEventExpose * event){
  GdkRectangle *rects;
  int nrects;
  gdk_region_get_rectangles(event->region, &rects, &nrects);
  for (int i = 0; i < nrects; ++i) {
    Rect<int> r(rects[i].x, rects[i].y, rects[i].width, rects[i].height);
    draw(r);
  }
  g_free(rects);
  return true;
}

bool SimpleViewer::on_button_press_event (GdkEventButton * event) {
  if (event->button == 1){
    drag_pos = Point<int>(event->x, event->y);
    on_drag=true;
  }
  return false;
}

bool SimpleViewer::on_button_release_event (GdkEventButton * event) {
  if (event->button == 1){
    get_window()->process_updates(false);
    on_drag=false;
  }
  return false;
}

bool SimpleViewer::on_motion_notify_event (GdkEventMotion * event) {
  int x=(int)event->x, y=(int)event->y;

  if ((event->state & Gdk::BUTTON1_MASK) &&
      (event->is_hint)){
    set_origin(origin - Point<int>(x,y) + drag_pos);
    drag_pos = Point<int>(x,y);
  }
  return false;
}

