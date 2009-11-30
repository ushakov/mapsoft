#include <iostream>
#include "../../core/lib2d/line.h"
#include "gobj.h"
#include "action_viewer.h"
#include "dthread_viewer.h"
#include "rubber.h"

class ActionTestCirc : public Action{
  public:
  ActionTestCirc(Rubber * r) : rubber(r) { }

  std::string get_name() { return "TestCirc"; }

  void init() {
    clear=true;
  }

  void reset() {
    rubber->clear();
    clear=true;
  }
  void click(const iPoint & p, const Gdk::ModifierType & state){
    if (clear){
      rubber->add_circc(p);
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
    ActionTestCirc A1(&rubber);
    actions.add(&A1);
    actions.select("TestCirc");

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
