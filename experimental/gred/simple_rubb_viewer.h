#include <gtkmm.h>
#include <gdk/gdk.h>

#include <list>

#include "gplane.h"
#include "rubber.h"
#include "simple_viewer.h"
#include "../../core/utils/image_gdk.h"

class SimpleRubbViewer : public SimpleViewer {
  public:

  std::list<RubberSegment> rubber, drawn;

  SimpleRubbViewer(GPlane * pl) : SimpleViewer(pl){}

  Point<GCoord> get_origin (void) {
    return origin;
  }

  virtual bool on_expose_event (GdkEventExpose * event){
    rubber_clear();
    SimpleViewer::on_expose_event(event);
    rubber_draw();
    return false;
  }

  virtual bool on_motion_notify_event (GdkEventMotion * event) {
    rubber_clear();
    SimpleViewer::on_motion_notify_event(event);
    rubber_draw();
    return false;
  }

  Glib::RefPtr<Gdk::GC> rubber_gc;

  void on_realize() {
    SimpleViewer::on_realize();
    rubber_gc = Gdk::GC::create(get_window());
    rubber_gc->set_rgb_fg_color(Gdk::Color("white"));
    rubber_gc->set_function(Gdk::XOR);
  }

  void rubber_draw(){
    Point<int> pointer;
    Gdk::ModifierType dummy1;
    get_window()->get_pointer(pointer.x, pointer.y, dummy1);

    for (std::list<RubberSegment>::const_iterator i = rubber.begin(); i != rubber.end(); i++){
      Point<GCoord> p1=i->get1(pointer, origin);
      Point<GCoord> p2=i->get2(pointer, origin);
      get_window()->draw_line(rubber_gc, p1.x, p1.y, p2.x, p2.y);
      drawn.push_back(RubberSegment(p1,0,p2,0));
    }
  }

  void rubber_clear(){
    Point<int> pointer;
    Gdk::ModifierType dummy1;
    get_window()->get_pointer(pointer.x, pointer.y, dummy1);

    for (std::list<RubberSegment>::const_iterator i = drawn.begin(); i != drawn.end(); i++){
      Point<GCoord> p1=i->p1;
      Point<GCoord> p2=i->p2;
      get_window()->draw_line(rubber_gc, p1.x, p1.y, p2.x, p2.y);
    }
    drawn.clear();
  }



};

