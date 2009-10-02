#include <iostream>
#include "../../core/lib2d/line.h"
#include "gplane.h"
#include "action_viewer.h"
#include "dthread_viewer.h"
#include "rubber_viewer.h"

typedef ActionViewer<RubberViewer<DThreadViewer> > ViewerT;

class ActionTestCirc : public Action{
  public:
  ActionTestCirc(ViewerT * v) : viewer(v) { }

  std::string get_name() { return "TestCirc"; }

  void init() {
    clear=true;
  }

  void reset() {
    viewer->rubber_clear();
    clear=true;
  }
  void click(const iPoint & p, const Gdk::ModifierType & state){
    if (clear){
      viewer->rubber_add_ellipse(p);
    } else {
      viewer->rubber_clear();
    }
    clear=!clear;
  }
  ViewerT * viewer;
  bool clear;
};

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GPlaneTestGridSlow p1;

    ViewerT viewer(&p1);
    ActionTestCirc A1(&viewer);
    viewer.action_add(&A1);
    viewer.action_select(0);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
