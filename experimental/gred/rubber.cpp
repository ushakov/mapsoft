#include "rubber.h"
#include <cassert>

#include "../../core/lib2d/point_utils.h" // for pdist()

RubberSegment::RubberSegment(
  const iPoint & p1_, const iPoint & p2_, const rubbfl_t flags_):
      p1(p1_), p2(p2_), flags(flags_){
}

void
RubberSegment::fix(Point<int> mouse, Point<int> origin){
  pf1 = iPoint ((flags & RUBBFL_MOUSE_P1X)? (p1.x+mouse.x) : (p1.x-origin.x),
                (flags & RUBBFL_MOUSE_P1Y)? (p1.y+mouse.y) : (p1.y-origin.y));
  pf2 = iPoint ((flags & RUBBFL_MOUSE_P2X)? (p2.x+mouse.x) : (p2.x-origin.x),
                (flags & RUBBFL_MOUSE_P2Y)? (p2.y+mouse.y) : (p2.y-origin.y));
}

Rubber::Rubber(Viewer * v): viewer(v){
  assert(viewer != NULL);

  /// We need to initialize our GC after viewer window will be set up.
  /// Connect this stuff to Gtk::Widget::signal_realize signal...
  viewer->signal_realize().connect_notify(
    sigc::mem_fun (*this, &Rubber::init_gc));

  /// Remove rubber before redraw, put it back after.
  viewer->signal_before_draw().connect(
    sigc::bind( sigc::mem_fun (*this, &Rubber::erase), true));
  viewer->signal_after_draw().connect(
    sigc::bind( sigc::mem_fun (*this, &Rubber::draw), true));

  /// Remove rubber before mouse motion, put it back after.
  viewer->signal_motion_notify_event().connect_notify(
    sigc::mem_fun (*this, &Rubber::before_motion_notify), false);
  viewer->signal_motion_notify_event().connect_notify(
    sigc::mem_fun (*this, &Rubber::after_motion_notify), true);

}

/// On mouse motions we need to redraw only mouse-connected lines
/// (false argument to draw/erase)
/// But in multy-thread viewers a full rubber redraw can occure
/// after such a partial erase then viewer is in drag mode.
/// So let's radraw all if viewer->on_drag. May be it is better
/// to make some lock here instead...
void
Rubber::before_motion_notify (GdkEventMotion * event) {
  if (!event->is_hint) return;
  mouse_pos=iPoint((int)event->x,(int)event->y);
  erase(viewer->is_on_drag());
}
void
Rubber::after_motion_notify (GdkEventMotion * event) {
  if (!event->is_hint) return;
  draw(viewer->is_on_drag());
}

/// This stuff must be done after viewer window is set up
void
Rubber::init_gc() {
  gc = Gdk::GC::create(viewer->get_window());
  gc->set_rgb_fg_color(Gdk::Color("white"));
  gc->set_function(Gdk::XOR);
}

/// Function for drawing single rubber segment.
/// Used by both draw() and erase()

void
Rubber::draw_segment(const RubberSegment &s){
  int r,w,h,x,y;
  if (!gc) return;
  switch (s.flags & RUBBFL_TYPEMASK){
     case RUBBFL_LINE:
       viewer->get_window()->draw_line(gc, s.pf1.x, s.pf1.y, s.pf2.x, s.pf2.y);
       break;
     case RUBBFL_ELL:
       w=abs(s.pf2.x-s.pf1.x);
       h=abs(s.pf2.y-s.pf1.y);
       x=s.pf1.x < s.pf2.x  ? s.pf1.x:s.pf2.x;
       y=s.pf1.y < s.pf2.y  ? s.pf1.y:s.pf2.y;
       viewer->get_window()->draw_arc(gc, false, x, y, w, h, 0, 360*64);
       break;
     case RUBBFL_ELLC:
       w=2*abs(s.pf2.x-s.pf1.x);
       h=2*abs(s.pf2.y-s.pf1.y);
       x=s.pf1.x-w;
       y=s.pf1.y-h;
       viewer->get_window()->draw_arc(gc, false, x, y, w, h, 0, 360*64);
       break;
     case RUBBFL_CIRC:
       w=(int)pdist(s.pf2, s.pf1);
       x=(s.pf1.x + s.pf2.x - w)/2;
       y=(s.pf1.y + s.pf2.y - w)/2;
       viewer->get_window()->draw_arc(gc, false, x, y, w, w, 0, 360*64);
       break;
     case RUBBFL_CIRCC:
       r=(int)pdist(s.pf2, s.pf1);
       x=s.pf1.x - r;
       y=s.pf1.y - r;
       viewer->get_window()->draw_arc(gc, false, x, y, 2*r, 2*r, 0, 360*64);
       break;
     default:
       std::cerr << "rubber: bad type: " << (s.flags & RUBBFL_TYPEMASK) << "\n";
  }
}

/// functions for drawing and erasing rubber
void
Rubber::draw(const bool all){
  std::list<RubberSegment>::iterator i;
  for (i = rubber.begin(); i != rubber.end(); i++){
    if ((!all && !(i->flags & RUBBFL_MOUSE)) ||
        (i->flags & RUBBFL_DRAWN)) continue; // already drawn

    i->fix(mouse_pos, viewer->get_origin());
    i->flags |= RUBBFL_DRAWN;
    draw_segment(*i);
  }
}

void
Rubber::erase(const bool all){
  std::list<RubberSegment>::iterator i;
  for (i = rubber.begin(); i != rubber.end(); i++){
    if ((!all && !(i->flags & RUBBFL_MOUSE)) ||
        !(i->flags & RUBBFL_DRAWN)) continue; // wasn't drawn
    draw_segment(*i);
    i->flags &= ~RUBBFL_DRAWN;
  }
}

/// add segment to a rubber
void
Rubber::add(const RubberSegment & s){
  rubber.push_back(s);
  rubber.rbegin()->fix(mouse_pos, viewer->get_origin());
  rubber.rbegin()->flags |= RUBBFL_DRAWN;
  draw_segment(*rubber.rbegin());
}

void
Rubber::add(const iPoint & p1, const iPoint & p2, const rubbfl_t flags){
  add(RubberSegment(p1, p2, flags));
}

void
Rubber::add(const int x1, const int y1,
                const int x2, const int y2, const rubbfl_t flags){
  add(RubberSegment(iPoint(x1,y1), Point<int>(x2,y2), flags));
}

/// remove the last segment from the rubber and get it
RubberSegment
Rubber::pop(void){
  if (rubber.size()<1) return RubberSegment(0,0,0);
  RubberSegment s = *rubber.rbegin();
  if (s.flags & RUBBFL_DRAWN) draw_segment(s); // erase segment
  rubber.pop_back();
  return s;
}

/// get the last segment from the rubber
RubberSegment
Rubber::get(void){
  if (rubber.size()<1) return RubberSegment(0,0,0);
  return *rubber.rbegin();
}

/// cleanup rubber
void
Rubber::clear(){
  erase();
  rubber.clear();
}

int
Rubber::size(){
  return rubber.size();
}

Rubber & Rubber::operator/= (double k){
  erase();
  for (std::list<RubberSegment>::iterator i = rubber.begin(); i != rubber.end(); ++i){
    if (!(i->flags & RUBBFL_MOUSE_P1X)) i->p1.x/=k;
    if (!(i->flags & RUBBFL_MOUSE_P1Y)) i->p1.y/=k;
    if (!(i->flags & RUBBFL_MOUSE_P2X)) i->p2.x/=k;
    if (!(i->flags & RUBBFL_MOUSE_P2Y)) i->p2.y/=k;
  }
  return *this;
}
Rubber & Rubber::operator*= (double k){
  erase();
  for (std::list<RubberSegment>::iterator i = rubber.begin(); i != rubber.end(); ++i){
    if (!(i->flags & RUBBFL_MOUSE_P1X)) i->p1.x*=k;
    if (!(i->flags & RUBBFL_MOUSE_P1Y)) i->p1.y*=k;
    if (!(i->flags & RUBBFL_MOUSE_P2X)) i->p2.x*=k;
    if (!(i->flags & RUBBFL_MOUSE_P2Y)) i->p2.y*=k;
  }
  return *this;
}

void Rubber::dump(void) const{
  std::cerr << "Rubber:\n";
  for (std::list<RubberSegment>::const_iterator i = rubber.begin(); i != rubber.end(); ++i){
    std::cerr << i->p1 << " " << i->p2 
              << " " << (i->flags & RUBBFL_MOUSE_P1X)
              << " " << (i->flags & RUBBFL_MOUSE_P2X)
              << " " << (i->flags & RUBBFL_MOUSE_P1Y)
              << " " << (i->flags & RUBBFL_MOUSE_P2Y)
              << " " << (i->flags >>4) 
              << "\n";
  }
  std::cerr << "-------\n";
}

/// High-level functions for adding some types of segments

void
Rubber::add_src_sq(const iPoint & p, int size){
  iPoint p1(size,size), p2(size,-size);
  add( p-p1, p-p2, RUBBFL_PLANE);
  add( p-p2, p+p1, RUBBFL_PLANE);
  add( p+p1, p+p2, RUBBFL_PLANE);
  add( p+p2, p-p1, RUBBFL_PLANE);
}

void
Rubber::add_dst_sq(int size){
  iPoint p1(-size,-size), p2(-size,size);
  iPoint p3( size, size), p4(size,-size);
  add( p1, p2, RUBBFL_MOUSE);
  add( p2, p3, RUBBFL_MOUSE);
  add( p3, p4, RUBBFL_MOUSE);
  add( p4, p1, RUBBFL_MOUSE);
}

void
Rubber::add_diag(const iPoint & p){
  add(p, iPoint(0,0), RUBBFL_MOUSE_P2);
}

void
Rubber::add_rect(const iPoint & p){
  add(iPoint(0,p.y), iPoint(0,0), RUBBFL_MOUSE_P1X | RUBBFL_MOUSE_P2);
  add(iPoint(p.x,0), iPoint(0,0), RUBBFL_MOUSE_P1Y | RUBBFL_MOUSE_P2);
  add(iPoint(0,p.y), p, RUBBFL_MOUSE_P1X);
  add(iPoint(p.x,0), p, RUBBFL_MOUSE_P1Y);
}

void
Rubber::add_ell(const iPoint & p){
  add(p, iPoint(0,0), RUBBFL_MOUSE_P2 | RUBBFL_ELL);
}

void
Rubber::add_ellc(const iPoint & p){
  add(p, iPoint(0,0), RUBBFL_MOUSE_P2 | RUBBFL_ELLC);
}

void
Rubber::add_circ(const iPoint & p){
  add(p, iPoint(0,0), RUBBFL_MOUSE_P2 | RUBBFL_CIRC);
}

void
Rubber::add_circc(const iPoint & p){
  add(p, iPoint(0,0), RUBBFL_MOUSE_P2 | RUBBFL_CIRCC);
}
