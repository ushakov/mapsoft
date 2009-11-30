#include <iostream>
#include "../../../core/lib2d/line.h"
#include "gobj.h"
#include "actions.h"
#include "dthread_viewer.h"
#include "rubber.h"

class ActionTest1 : public Action{
  public:
  ActionTest1(Rubber * r) : rubber(r) { }

  std::string get_name() { return "Test1"; }

  void init() {
    clear=true;
  }

  void reset() {
    rubber->clear();
    clear=true;
  }
  void click(const iPoint & p, const Gdk::ModifierType & state){
    if (clear){
      rubber->add_src_sq(p, 3);
      rubber->add_dst_sq(3);
      rubber->add_rect(p);
      rubber->add_diag(p);
    } else {
      rubber->clear();
    }
    clear=!clear;
  }
  Rubber * rubber;
  bool clear;
};

class ActionTest2 : public Action{
  public:
  ActionTest2(Rubber * r) : rubber(r) { }

  std::string get_name() { return "Test2"; }

  void init() {
    std::cout << "Test action for drawing polylines.\n"
              << " * left button: add points\n"
              << " * control + left button: remove last point\n"
              << " * middle button: finish line, print it to stdout\n"
              << " * right button:  abort action\n";
  }

  void reset() {
    rubber->clear();
    data.clear();
  }

  void click(const iPoint & p, const Gdk::ModifierType & state){

    rubber->clear();
    if (state&Gdk::BUTTON1_MASK){
      if (state&Gdk::CONTROL_MASK){
        if (data.size()>0) data.resize(data.size()-1);
      }
      else data.push_back(p);
      if (data.size()>0) rubber->add_diag(data[data.size()-1]);
      for (int i=0; i+1<data.size(); i++){
        rubber->add(data[i], data[i+1], RUBBFL_PLANE);
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
  Rubber * rubber;
  iLine data;
  bool clear;
};

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestGridSlow p1;

    DThreadViewer viewer(&p1);
    Rubber  rubber(&viewer);
    Actions actions(&viewer);

    ActionTest1 A1(&rubber);
    ActionTest2 A2(&rubber);
    actions.add(&A1);
    actions.add(&A2);
    actions.select("Test2");

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
