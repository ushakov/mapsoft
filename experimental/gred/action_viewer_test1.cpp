#include <iostream>
#include "../../core/lib2d/line.h"
#include "gplane.h"
#include "action_viewer.h"
#include "dthread_viewer.h"
#include "rubber_viewer.h"

typedef ActionViewer<RubberViewer<DThreadViewer> > ViewerT;

class ActionTest1 : public Action{
  public:
  ActionTest1(ViewerT * v) : viewer(v) { }

  std::string get_name() { return "Test1"; }

  virtual void init() {
    clear=true;
  }
  virtual void reset() {
    viewer->rubber_clear();
    clear=true;
  }
  virtual void click(iPoint p) {
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

class ActionTest2 : public Action{
  public:
  ActionTest2(ViewerT * v) : viewer(v) { }

  std::string get_name() { return "Test2"; }

  virtual void init() {
    viewer->rubber_clear();
    data.clear();
  }
  virtual void reset() {
    viewer->rubber_clear();
    data.clear();
  }
  virtual void click(iPoint p) {
    int x,y;
    Gdk::ModifierType state;
    viewer->rubber_clear();

    viewer->get_window()->get_pointer(x,y,state);
    if (state&Gdk::CONTROL_MASK){
      if (data.size()>0) data.resize(data.size()-1);
    }
    else data.push_back(p);
    if (data.size()>0) viewer->rubber_add_diag(data[data.size()-1]);
    for (int i=0; i+1<data.size(); i++){
      viewer->rubber_add(data[i], 0, data[i+1], 0);
    }
  }
  ViewerT * viewer;
  iLine data;
  bool clear;
};

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GPlaneTestGridSlow p1;

    ViewerT viewer(&p1);
    ActionTest1 A1(&viewer);
    ActionTest2 A2(&viewer);
    viewer.action_add(&A1);
    viewer.action_add(&A2);
    viewer.action_select(1);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
