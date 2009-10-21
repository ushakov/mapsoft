#ifndef RUBBER_VIEWER_H 
#define RUBBER_VIEWER_H 

#include <gtkmm.h>
#include <gdk/gdk.h>

#include <list>
#include "gobj.h"
#include "../../core/lib2d/point_utils.h"

typedef unsigned int rubbfl_t;
#define RUBBFL_PLANE     0
#define RUBBFL_MOUSE_P1X 1
#define RUBBFL_MOUSE_P1Y 2
#define RUBBFL_MOUSE_P1  3
#define RUBBFL_MOUSE_P2X 4
#define RUBBFL_MOUSE_P2Y 8
#define RUBBFL_MOUSE_P2  0xC
#define RUBBFL_MOUSE     0xF

#define RUBBFL_TYPEMASK 0xF0
#define RUBBFL_LINE  0x00
#define RUBBFL_ELL   0x10
#define RUBBFL_ELLC  0x20
#define RUBBFL_CIRC  0x30
#define RUBBFL_CIRCC 0x40

class RubberSegment{
public:
    rubbfl_t flags;
    iPoint p1, p2;

    RubberSegment(const iPoint & p1_, const iPoint & p2_, const rubbfl_t flags_):
      p1(p1_), p2(p2_), flags(flags_){ }

    iPoint get1(Point<int> pointer, Point<int> offset) const {
      return iPoint ((flags & RUBBFL_MOUSE_P1X)? (p1.x+pointer.x) : (p1.x-offset.x),
                     (flags & RUBBFL_MOUSE_P1Y)? (p1.y+pointer.y) : (p1.y-offset.y));
    }
    iPoint get2(Point<int> pointer, Point<int> offset) const {
      return iPoint ((flags & RUBBFL_MOUSE_P2X)? (p2.x+pointer.x) : (p2.x-offset.x),
                     (flags & RUBBFL_MOUSE_P2Y)? (p2.y+pointer.y) : (p2.y-offset.y));
    }
};

template <typename ViewerT>
class RubberViewer : public ViewerT {
  private:
  std::list<RubberSegment> rubber, drawn;
  iPoint mouse_pos;

  public:
  RubberViewer(GObj * pl) : ViewerT(pl){}

  // There are three places in which we need to redraw rubber lines:
  // * draw_image function
  // * mouse movements without plane movements
  // * plane movements

  virtual void draw_image (const iImage & img, const iPoint & p){
    rubber_erase();
    ViewerT::draw_image(img, p);
    rubber_draw();
  }

  virtual void draw_image (const iImage & img, const iRect & part, const iPoint & p){
    rubber_erase();
    ViewerT::draw_image(img, part, p);
    rubber_draw();
  }

  virtual bool on_motion_notify_event (GdkEventMotion * event) {
    if (!event->is_hint) return false;
    mouse_pos=iPoint((int)event->x,(int)event->y);
    rubber_erase(false);
    if (ViewerT::on_drag){
      ViewerT::set_origin(ViewerT::get_origin() - mouse_pos + ViewerT::drag_pos);
      ViewerT::drag_pos = mouse_pos;
    }
    rubber_draw(false);
    return false;
  }

  Glib::RefPtr<Gdk::GC> rubber_gc;

  void on_realize() {
    ViewerT::on_realize();
    rubber_gc = Gdk::GC::create(ViewerT::get_window());
    rubber_gc->set_rgb_fg_color(Gdk::Color("white"));
    rubber_gc->set_function(Gdk::XOR);
  }

  void rubber_draw_segment(const RubberSegment &s){
    int r,w,h,x,y;
    switch (s.flags & RUBBFL_TYPEMASK){
       case RUBBFL_LINE:
         ViewerT::get_window()->draw_line(rubber_gc, s.p1.x, s.p1.y, s.p2.x, s.p2.y);
         break;
       case RUBBFL_ELL:
         w=abs(s.p2.x-s.p1.x);
         h=abs(s.p2.y-s.p1.y);
         x=s.p1.x < s.p2.x  ? s.p1.x:s.p2.x;
         y=s.p1.y < s.p2.y  ? s.p1.y:s.p2.y;
         ViewerT::get_window()->draw_arc(rubber_gc, false, x, y, w, h, 0, 360*64);
         break;
       case RUBBFL_ELLC:
         w=2*abs(s.p2.x-s.p1.x);
         h=2*abs(s.p2.y-s.p1.y);
         x=s.p1.x-w;
         y=s.p1.y-h;
         ViewerT::get_window()->draw_arc(rubber_gc, false, x, y, w, h, 0, 360*64);
         break;
       case RUBBFL_CIRC:
         w=pdist(s.p2, s.p1);
         x=(s.p1.x + s.p2.x - w)/2;
         y=(s.p1.y + s.p2.y - w)/2;
         ViewerT::get_window()->draw_arc(rubber_gc, false, x, y, w, w, 0, 360*64);
         break;
       case RUBBFL_CIRCC:
         r=pdist(s.p2, s.p1);
         x=s.p1.x - r;
         y=s.p1.y - r;
         ViewerT::get_window()->draw_arc(rubber_gc, false, x, y, 2*r, 2*r, 0, 360*64);
         break;
       default:
         std::cerr << "rubber_viewer: bad flags: " << s.flags << "\n";
    }
  }

  void rubber_draw(const bool all=true){
    // if all is false - draw only lines connected with mouse
    if (!rubber_gc) return;
    for (std::list<RubberSegment>::const_iterator i = rubber.begin(); i != rubber.end(); i++){
      if (!all && !(i->flags & RUBBFL_MOUSE)) continue;
      iPoint p1=i->get1(mouse_pos, ViewerT::get_origin());
      iPoint p2=i->get2(mouse_pos, ViewerT::get_origin());
      RubberSegment s(p1,p2,i->flags);
      rubber_draw_segment(s);
      drawn.push_back(s);
    }
  }

  void rubber_erase(const bool all=true){
    if (!rubber_gc) return;
    std::list<RubberSegment>::iterator i = drawn.begin();
    while (i != drawn.end()){
      if (!all && !(i->flags & RUBBFL_MOUSE)) {i++; continue;}
      rubber_draw_segment(*i);
      i=drawn.erase(i);
    }
  }

  void rubber_add(const iPoint & p1, const iPoint & p2, const rubbfl_t flags){
    rubber_erase();
    rubber.push_back(RubberSegment(p1, p2, flags));
    rubber_draw();
  }

  void rubber_add(const int x1, const int y1,
                  const int x2, const int y2, const rubbfl_t flags){
    rubber_add(iPoint(x1,y1), Point<int>(x2,y2), flags);
  }

  void rubber_clear(){
    rubber_erase();
    rubber.clear();
  }

  void rubber_add_src_sq(const iPoint & p, int size){
      iPoint p1(size,size), p2(size,-size);
      rubber_add( p-p1, p-p2, RUBBFL_PLANE);
      rubber_add( p-p2, p+p1, RUBBFL_PLANE);
      rubber_add( p+p1, p+p2, RUBBFL_PLANE);
      rubber_add( p+p2, p-p1, RUBBFL_PLANE);
  }

  void rubber_add_dst_sq(int size){
      iPoint p1(-size,-size), p2(-size,size);
      iPoint p3( size, size), p4(size,-size);
      rubber_add( p1, p2, RUBBFL_MOUSE);
      rubber_add( p2, p3, RUBBFL_MOUSE);
      rubber_add( p3, p4, RUBBFL_MOUSE);
      rubber_add( p4, p1, RUBBFL_MOUSE);
  }

  void rubber_add_diag(const iPoint & p){
      rubber_add(p, iPoint(0,0), RUBBFL_MOUSE_P2);
  }

  void rubber_add_rect(const iPoint & p){
      rubber_add(iPoint(0,p.y), iPoint(0,0), RUBBFL_MOUSE_P1X | RUBBFL_MOUSE_P2);
      rubber_add(iPoint(p.x,0), iPoint(0,0), RUBBFL_MOUSE_P1Y | RUBBFL_MOUSE_P2);
      rubber_add(iPoint(0,p.y), p, RUBBFL_MOUSE_P1X);
      rubber_add(iPoint(p.x,0), p, RUBBFL_MOUSE_P1Y);
  }

  void rubber_add_ell(const iPoint & p){
      rubber_add(p, iPoint(0,0), RUBBFL_MOUSE_P2 | RUBBFL_ELL);
  }
  void rubber_add_ellc(const iPoint & p){
      rubber_add(p, iPoint(0,0), RUBBFL_MOUSE_P2 | RUBBFL_ELLC);
  }
  void rubber_add_circ(const iPoint & p){
      rubber_add(p, iPoint(0,0), RUBBFL_MOUSE_P2 | RUBBFL_CIRC);
  }
  void rubber_add_circc(const iPoint & p){
      rubber_add(p, iPoint(0,0), RUBBFL_MOUSE_P2 | RUBBFL_CIRCC);
  }

};

#endif
