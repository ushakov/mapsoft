#include <iostream>
#include "../../core/lib2d/line.h"
#include "gobj.h"
#include "action_viewer.h"
#include "dthread_viewer.h"
#include "rubber.h"

typedef ActionViewer<DThreadViewer> ViewerT;

class ActionTest1 : public Action{
  public:
  ActionTest1(ViewerT * v, Rubber * r) : viewer(v), rubber(r) { }

  std::string get_name() { return "Test1"; }

  void init() {
    clear=true;
  }

  void reset() {
    rubber->rubber_clear();
    clear=true;
  }
  void click(const iPoint & p, const Gdk::ModifierType & state){
    if (clear){
      rubber->rubber_add_src_sq(p, 3);
      rubber->rubber_add_dst_sq(3);
      rubber->rubber_add_rect(p);
      rubber->rubber_add_diag(p);
    } else {
      rubber->rubber_clear();
    }
    clear=!clear;
  }
  ViewerT * viewer;
  Rubber * rubber;
  bool clear;
};

class ActionTest2 : public Action{
  public:
  ActionTest2(ViewerT * v, Rubber * r) : viewer(v), rubber(r) { }

  std::string get_name() { return "Test2"; }

  void init() {
    std::cout << "Test action for drawing polylines.\n"
              << " * left button: add points\n"
              << " * control + left button: remove last point\n"
              << " * middle button: finish line, print it to stdout\n"
              << " * right button:  abort action\n";
  }

  void reset() {
    rubber->rubber_clear();
    data.clear();
  }

  void click(const iPoint & p, const Gdk::ModifierType & state){

    rubber->rubber_clear();
    if (state&Gdk::BUTTON1_MASK){
      if (state&Gdk::CONTROL_MASK){
        if (data.size()>0) data.resize(data.size()-1);
      }
      else data.push_back(p);
      if (data.size()>0) rubber->rubber_add_diag(data[data.size()-1]);
      for (int i=0; i+1<data.size(); i++){
        rubber->rubber_add(data[i], data[i+1], RUBBFL_PLANE);
      }
    }
    else if (state&Gdk::BUTTON2_MASK){
      data.push_back(p);
      std::cout << data << "\n";
      data.clear();
    }
    else if (state&Gdk::BUTTON3_MASK){
      data.clear();
    }
  }
  ViewerT * viewer;
  Rubber * rubber;
  iLine data;
  bool clear;
};

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestGridSlow p1;

    ViewerT viewer(&p1);
    Rubber rubber(&viewer);

    ActionTest1 A1(&viewer, &rubber);
    ActionTest2 A2(&viewer, &rubber);
    viewer.action_add(&A1);
    viewer.action_add(&A2);
    viewer.action_select(1);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
