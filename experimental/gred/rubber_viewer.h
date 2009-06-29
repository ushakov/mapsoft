#ifndef RUBBER_VIEWER_H 
#define RUBBER_VIEWER_H 

#include <gtkmm.h>
#include <gdk/gdk.h>

#include <list>
#include "gplane.h"

const int REF_XMOUSE=1;
const int REF_YMOUSE=2;

class RubberSegment{
public:
    int        r1, r2;
    iPoint p1, p2;

    RubberSegment(
      const iPoint & p1_, const int r1_,
      const iPoint & p2_, const int r2_): p1(p1_), r1(r1_), p2(p2_), r2(r2_){ }

    iPoint get1(Point<int> pointer, Point<int> offset) const {
      return iPoint ((r1 & REF_XMOUSE)? (p1.x+pointer.x) : (p1.x-offset.x),
                            (r1 & REF_YMOUSE)? (p1.y+pointer.y) : (p1.y-offset.y));
    }
    iPoint get2(Point<int> pointer, Point<int> offset) const {
      return iPoint ((r2 & REF_XMOUSE)? (p2.x+pointer.x) : (p2.x-offset.x),
                            (r2 & REF_YMOUSE)? (p2.y+pointer.y) : (p2.y-offset.y));
    }
};

template <typename ViewerT>
class RubberViewer : public ViewerT {
  private:
  std::list<RubberSegment> rubber, drawn;
  iPoint mouse_pos;

  public:
  RubberViewer(GPlane * pl) : ViewerT(pl){}

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
    rubber_erase();
    if (ViewerT::on_drag){
      ViewerT::set_origin(ViewerT::get_origin() - mouse_pos + ViewerT::drag_pos);
      ViewerT::drag_pos = mouse_pos;
    }
    rubber_draw();
    return false;
  }

  Glib::RefPtr<Gdk::GC> rubber_gc;

  void on_realize() {
    ViewerT::on_realize();
    rubber_gc = Gdk::GC::create(ViewerT::get_window());
    rubber_gc->set_rgb_fg_color(Gdk::Color("white"));
    rubber_gc->set_function(Gdk::XOR);
  }

  void rubber_draw(){
    if (!rubber_gc) return;
    for (std::list<RubberSegment>::const_iterator i = rubber.begin(); i != rubber.end(); i++){
      iPoint p1=i->get1(mouse_pos, ViewerT::get_origin());
      iPoint p2=i->get2(mouse_pos, ViewerT::get_origin());
      ViewerT::get_window()->draw_line(rubber_gc, p1.x, p1.y, p2.x, p2.y);
      drawn.push_back(RubberSegment(p1,0,p2,0));
    }
  }

  void rubber_erase(){
    if (!rubber_gc) return;
    for (std::list<RubberSegment>::const_iterator i = drawn.begin(); i != drawn.end(); i++){
      iPoint p1=i->p1;
      iPoint p2=i->p2;
      ViewerT::get_window()->draw_line(rubber_gc, p1.x, p1.y, p2.x, p2.y);
    }
    drawn.clear();
  }

  void rubber_add(const iPoint & p1, const int r1,
                  const iPoint & p2, const int r2){
    rubber_erase();
    rubber.push_back(RubberSegment(p1, r1, p2, r2));
    rubber_draw();
  }

  void rubber_add(const int x1, const int y1, const int r1,
                  const int x2, const int y2, const int r2){
    rubber_add(iPoint(x1,y1), r1, Point<int>(x2,y2),r2);
  }

  void rubber_clear(){
    rubber_erase();
    rubber.clear();
  }


};

#endif
