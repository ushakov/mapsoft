#include <iostream>
#include "gplane.h"
#include "action_viewer.h"
#include "dthread_viewer.h"
#include "rubber_viewer.h"

typedef ActionViewer<RubberViewer<DThreadViewer> > ViewerT;

class ActionTest : public Action{
  public:
  ActionTest(ViewerT * v) : viewer(v), clear(true){ }

  std::string get_name() { return "Test1"; }
  virtual void init() {
    std::cerr << "ActionTest: init\n";
  }
  virtual void reset() {
    std::cerr << "ActionTest: reset\n";
    viewer->rubber_clear();
  }
  virtual void click(iPoint p) {
    std::cerr << "ActionTest: click\n";
    if (clear){
      viewer->rubber_add_src_sq(p, 3);
      viewer->rubber_add_dst_sq(3);
      viewer->rubber_add_rect(p);
      viewer->rubber_add_diag(p);
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
    ActionTest A1(&viewer);
    viewer.action_add(&A1);
    viewer.action_select(0);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
