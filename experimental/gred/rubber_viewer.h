#include <gtkmm.h>
#include <gdk/gdk.h>

#include <list>

const int REF_XMOUSE=1;
const int REF_YMOUSE=2;

class RubberSegment{
public:
    int        r1, r2;
    Point<int> p1, p2;

    RubberSegment(
      const Point<int> & p1_, const int r1_,
      const Point<int> & p2_, const int r2_): p1(p1_), r1(r1_), p2(p2_), r2(r2_){ }

    Point<int> get1(Point<int> pointer, Point<int> offset) const {
      return Point<int> ((r1 & REF_XMOUSE)? (p1.x+pointer.x) : (p1.x-offset.x),
                            (r1 & REF_YMOUSE)? (p1.y+pointer.y) : (p1.y-offset.y));
    }
    Point<int> get2(Point<int> pointer, Point<int> offset) const {
      return Point<int> ((r2 & REF_XMOUSE)? (p2.x+pointer.x) : (p2.x-offset.x),
                            (r2 & REF_YMOUSE)? (p2.y+pointer.y) : (p2.y-offset.y));
    }
};

template <typename ViewerT>
class RubberViewer : public ViewerT {
  public:

  std::list<RubberSegment> rubber, drawn;

  RubberViewer(GPlane * pl) : ViewerT(pl){}

  virtual bool on_expose_event (GdkEventExpose * event){
    rubber_clear();
    ViewerT::on_expose_event(event);
    rubber_draw();
    return false;
  }

  virtual bool on_motion_notify_event (GdkEventMotion * event) {
    if (!ViewerT::on_drag) rubber_clear();
    int x=(int)event->x, y=(int)event->y;
    if (ViewerT::on_drag && event->is_hint){
      mouse_pos=Point<int>(x,y);
      ViewerT::set_origin(ViewerT::origin - Point<int>(x,y) + ViewerT::drag_pos);
      ViewerT::drag_pos = Point<int>(x,y);
    }
    if (!ViewerT::on_drag){
      ViewerT::get_pointer(mouse_pos.x, mouse_pos.y);
      rubber_draw();
    }
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
    for (std::list<RubberSegment>::const_iterator i = rubber.begin(); i != rubber.end(); i++){
      Point<int> p1=i->get1(mouse_pos, ViewerT::origin);
      Point<int> p2=i->get2(mouse_pos, ViewerT::origin);
      ViewerT::get_window()->draw_line(rubber_gc, p1.x, p1.y, p2.x, p2.y);
      drawn.push_back(RubberSegment(p1,0,p2,0));
    }
  }

  void rubber_clear(){
    for (std::list<RubberSegment>::const_iterator i = drawn.begin(); i != drawn.end(); i++){
      Point<int> p1=i->p1;
      Point<int> p2=i->p2;
      ViewerT::get_window()->draw_line(rubber_gc, p1.x, p1.y, p2.x, p2.y);
    }
    drawn.clear();
  }

  void rubber_add(const int x1, const int y1, const int r1,
                  const int x2, const int y2, const int r2){
    rubber.push_back(RubberSegment(Point<int>(x1,y1), r1, Point<int>(x2,y2),r2));
  }
  void rubber_add(const Point<int> & p1, const int r1,
                  const Point<int> & p2, const int r2){
    rubber.push_back(RubberSegment(p1, r1, p2, r2));
  }

  Point<int> mouse_pos;

};

