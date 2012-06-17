#include "simple_viewer.h"

#include <gdk/gdk.h>
#include <cassert>
#include "utils/image_gdk.h"
#include "2d/rect.h"

SimpleViewer::SimpleViewer(GObj * o) :
    obj(o),
    origin(iPoint(0,0)),
    on_drag(false),
    epoch(0),
    bgcolor(0xFF000000),
    sc(1.0) {
  set_name("MapsoftViewer");
  set_events (
    Gdk::BUTTON_PRESS_MASK |
    Gdk::BUTTON_RELEASE_MASK |
    Gdk::SCROLL_MASK |
    Gdk::POINTER_MOTION_MASK |
    Gdk::POINTER_MOTION_HINT_MASK );
}

/***********************************************************/

void
SimpleViewer::set_origin (iPoint p) {
  iRect r = range();
  int w=get_width();
  int h=get_height();

  if (get_xloop()){
    p.x = p.x % r.w;
  }
  else {
    if (p.x + w >= r.x + r.w) p.x = r.x + r.w - w - 1;
    if (p.x < r.x) p.x=r.x;
  }

  if (get_yloop()){
    p.y = p.y % r.h;
  }
  else {
    if (p.y + h >= r.y + r.h) p.y = r.y+ r.h - h - 1;
    if (p.y < r.y) p.y=r.y;
  }

  if (is_realized()){
    get_window()->scroll(origin.x-p.x, origin.y-p.y);
  }
  origin = p;
  signal_ch_origin_.emit(p);
}

void
SimpleViewer::set_center (iPoint p) {
  set_origin(p - iPoint(get_width(), get_height())/2);
}

iPoint
SimpleViewer::get_origin (void) const {
  return origin;
}

iPoint
SimpleViewer::get_center (void) const {
  return origin + iPoint(get_width(), get_height())/2;
}

void
SimpleViewer::set_obj (GObj * o){
  obj=o;
  redraw();
}

GObj *
SimpleViewer::get_obj (void) const{
  return obj;
}

void
SimpleViewer::set_bgcolor (int c){
  bgcolor=c | 0xFF000000;
}

int
SimpleViewer::get_bgcolor (void) const{
  return bgcolor;
}


bool
SimpleViewer::get_xloop() const{
  return obj?obj->get_xloop():false;
}

bool
SimpleViewer::get_yloop() const{
  return obj?obj->get_yloop():false;
}

iRect
SimpleViewer::range() const{
  return obj?obj->range():GOBJ_MAX_RANGE;
}

/***********************************************************/

void
SimpleViewer::draw(const iRect & r){
  if (r.empty()) return;
  signal_busy_.emit();
  iImage img(r.w, r.h, 0xFF000000 | bgcolor);
  if (obj) obj->draw(img, r.TLC()+origin);
  draw_image(img, r.TLC());
  signal_idle_.emit();
}

void
SimpleViewer::draw_image (const iImage & img, const iPoint & p){
  if (!is_realized()) return;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(img);
  Glib::RefPtr<Gdk::GC> gc = get_style()->get_fg_gc (get_state());
  Glib::RefPtr<Gdk::Window> widget = get_window();
  signal_before_draw_.emit();
  widget->draw_pixbuf(gc, pixbuf,
          0,0,  p.x, p.y,  img.w, img.h,
          Gdk::RGB_DITHER_NORMAL, 0, 0);
  signal_after_draw_.emit();
}

void
SimpleViewer::draw_image (const iImage & img, const iRect & part, const iPoint & p){
  if (!is_realized()) return;
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(img);
  Glib::RefPtr<Gdk::GC> gc = get_style()->get_fg_gc (get_state());
  Glib::RefPtr<Gdk::Window> widget = get_window();
  signal_before_draw_.emit();
  widget->draw_pixbuf(gc, pixbuf,
          part.x, part.y,  p.x, p.y,  part.w, part.h,
          Gdk::RGB_DITHER_NORMAL, 0, 0);
  signal_after_draw_.emit();
}

/***********************************************************/

void
SimpleViewer::redraw (void){
  epoch++;
  draw(iRect(0, 0, get_width(), get_height()));
}

void
SimpleViewer::rescale(const double k){
  rescale(k,iPoint(get_width(), get_height())/2);
}

void
SimpleViewer::rescale(const double k, const iPoint & cnt){
  signal_on_rescale_.emit(k);
  if (obj){
    obj->get_cnv()->rescale_src(k);
    obj->refresh();
  }
  iPoint wsize(get_width(), get_height());
  iPoint wcenter = get_origin() + cnt;
  wcenter=iPoint(wcenter.x * k, wcenter.y * k);
  set_origin(wcenter - cnt);
  redraw();
}

/***********************************************************/

bool
SimpleViewer::on_expose_event (GdkEventExpose * event){
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

bool
SimpleViewer::on_button_press_event (GdkEventButton * event) {
  if ((event->button == 1) || (event->button == 2)){
    drag_pos = iPoint((int)event->x, (int)event->y);
    on_drag=true;
  }
  return false;
}

bool
SimpleViewer::on_button_release_event (GdkEventButton * event) {
  if ((event->button == 1) || (event->button == 2)){
    on_drag=false;
  }
  return false;
}

bool
SimpleViewer::on_motion_notify_event (GdkEventMotion * event) {
  if (!event->is_hint) return false;
  if (on_drag){
    iPoint p((int)event->x, (int)event->y);
    set_origin(origin - p + drag_pos);
    drag_pos = p;
  }
  return false;
}

/***********************************************************/

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

/***********************************************************/

sigc::signal<void> &
SimpleViewer::signal_before_draw(){ return signal_before_draw_;}

sigc::signal<void> &
SimpleViewer::signal_after_draw(){ return signal_after_draw_;}

sigc::signal<void> &
SimpleViewer::signal_busy(){ return signal_busy_;}

sigc::signal<void> &
SimpleViewer::signal_idle(){ return signal_idle_;}

sigc::signal<void, double> &
SimpleViewer::signal_on_rescale(){ return signal_on_rescale_;}

sigc::signal<void, iPoint> &
SimpleViewer::signal_ch_origin(){ return signal_ch_origin_;}
