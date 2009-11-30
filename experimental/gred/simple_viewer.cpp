#include "simple_viewer.h"

#include <gdk/gdk.h>
#include <cassert>
#include "../../core/utils/image_gdk.h"
#include "../../core/lib2d/rect.h"

SimpleViewer::SimpleViewer(GObj * o) :
    obj(o),
    origin(iPoint(0,0)),
    on_drag(false),
    epoch(0) {
  set_name("MapsoftViewer");
  set_events (
    Gdk::BUTTON_PRESS_MASK |
    Gdk::BUTTON_RELEASE_MASK |
    Gdk::SCROLL_MASK |
    Gdk::POINTER_MOTION_MASK |
    Gdk::POINTER_MOTION_HINT_MASK );
}

void SimpleViewer::set_origin (iPoint p) {
  assert(obj !=NULL);
  iRect r=obj->range();
  int w=get_width();
  int h=get_height();

  if (p.x + w >= r.x + r.w) p.x = r.x + r.w - w - 1;
  if (p.y + h >= r.y + r.h) p.y = r.y+ r.h - h - 1;
  if (p.x < r.x) p.x=r.x;
  if (p.y < r.y) p.y=r.y;

  get_window()->scroll(origin.x-p.x, origin.y-p.y);
  origin = p;
}

iPoint SimpleViewer::get_origin (void) const {
  return origin;
}

void SimpleViewer::set_obj (GObj * o){
  obj=o;
}

GObj * SimpleViewer::get_obj (void) const{
  return obj;
}

void SimpleViewer::redraw (void){
  epoch++;
  draw(iRect(0, 0, get_width(), get_height()));
}

void SimpleViewer::draw(const iRect & r){
  if (r.empty()) return;
  draw_image(obj->draw(r + origin), r.TLC());
}

void SimpleViewer::draw_image (const iImage & img, const iPoint & p){
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(img);
  Glib::RefPtr<Gdk::GC> gc = get_style()->get_fg_gc (get_state());
  Glib::RefPtr<Gdk::Window> widget = get_window();
  signal_before_draw.emit();
  widget->draw_pixbuf(gc, pixbuf,
          0,0,  p.x, p.y,  img.w, img.h,
          Gdk::RGB_DITHER_NORMAL, 0, 0);
  signal_after_draw.emit();
}

void SimpleViewer::draw_image (const iImage & img, const iRect & part, const iPoint & p){
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(img);
  Glib::RefPtr<Gdk::GC> gc = get_style()->get_fg_gc (get_state());
  Glib::RefPtr<Gdk::Window> widget = get_window();
  signal_before_draw.emit();
  widget->draw_pixbuf(gc, pixbuf,
          part.x, part.y,  p.x, p.y,  part.w, part.h,
          Gdk::RGB_DITHER_NORMAL, 0, 0);
  signal_after_draw.emit();
}

bool SimpleViewer::on_expose_event (GdkEventExpose * event){
  GdkRectangle *rects;
  int nrects;
  gdk_region_get_rectangles(event->region, &rects, &nrects);
  for (int i = 0; i < nrects; ++i) {
    iRect r(rects[i].x, rects[i].y, rects[i].width, rects[i].height);
    draw(r);
  }
  g_free(rects);
  return true;
}

bool SimpleViewer::on_button_press_event (GdkEventButton * event) {
  if ((event->button == 1) || (event->button == 2)){
    drag_pos = iPoint((int)event->x, (int)event->y);
    on_drag=true;
  }
  else if (event->button == 3) {
    redraw();
  }
  return false;
}

bool SimpleViewer::on_button_release_event (GdkEventButton * event) {
  if ((event->button == 1) || (event->button == 2)){
    on_drag=false;
  }
  return false;
}

bool SimpleViewer::on_motion_notify_event (GdkEventMotion * event) {
  if (!event->is_hint) return false;
  if (on_drag){
    iPoint p((int)event->x, (int)event->y);
    set_origin(origin - p + drag_pos);
    drag_pos = p;
  }
  return false;
}

bool
SimpleViewer::is_on_drag(){
  return on_drag;
}

int
SimpleViewer::get_epoch(){
  return epoch;
}

void
SimpleViewer::inc_epoch(){
  epoch++;
}
