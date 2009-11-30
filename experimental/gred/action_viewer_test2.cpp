#include <iostream>
#include "gobj.h"
#include "action_viewer.h"
#include "dthread_viewer.h"
#include "rubber.h"

class ActionTest : public Action{
  public:
  ActionTest(Rubber * r) : rubber(r) { }

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

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestGridSlow p1;

    DThreadViewer viewer(&p1);
    Rubber  rubber(&viewer);
    Actions actions(&viewer);
    ActionTest A1(&rubber);
    actions.add(&A1);
    actions.select("Test1");

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
