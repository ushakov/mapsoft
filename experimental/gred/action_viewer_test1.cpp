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
      iPoint p0(0,0), p1(5,5), p2(5,-5);
      viewer->rubber_add( p-p1,  0,   p-p2, 0);
      viewer->rubber_add( p-p2,  0,   p+p1, 0);
      viewer->rubber_add( p+p1,  0,   p+p2, 0);
      viewer->rubber_add( p+p2,  0,   p-p1, 0);

      viewer->rubber_add( p,  0,   p0, 3);

      viewer->rubber_add( iPoint(0,p.y),  1,   p0, 3);
      viewer->rubber_add( iPoint(p.x,0),  2,   p0, 3);
      viewer->rubber_add( p,  0,   iPoint(0,p.y), 1);
      viewer->rubber_add( p,  0,   iPoint(p.x,0), 2);

      viewer->rubber_add(p0-p1,  3,  p0-p2, 3);
      viewer->rubber_add(p0-p2,  3,     p1, 3);
      viewer->rubber_add(p1,     3,     p2, 3);
      viewer->rubber_add(p2,     3,  p0-p1, 3);
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
