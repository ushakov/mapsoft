#include <iostream>
#include "../../core/lib2d/line.h"
#include "gobj.h"
#include "action_viewer.h"
#include "dthread_viewer.h"
#include "rubber.h"

typedef ActionViewer<DThreadViewer> ViewerT;

class ActionTestCirc : public Action{
  public:
  ActionTestCirc(ViewerT * v, Rubber * r) : viewer(v), rubber(r) { }

  std::string get_name() { return "TestCirc"; }

  void init() {
    clear=true;
  }

  void reset() {
    rubber->rubber_clear();
    clear=true;
  }
  void click(const iPoint & p, const Gdk::ModifierType & state){
    if (clear){
      rubber->rubber_add_circc(p);
    } else {
      rubber->rubber_clear();
    }
    clear=!clear;
  }
  ViewerT * viewer;
  Rubber * rubber;
  bool clear;
};

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestGridSlow p1;

    ViewerT viewer(&p1);
    Rubber rubber(&viewer);
    ActionTestCirc A1(&viewer, &rubber);
    viewer.action_add(&A1);
    viewer.action_select(0);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
