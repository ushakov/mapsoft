#include <iostream>
#include "dthread_viewer.h"
#include "mthread_viewer.h"
#include "simple_viewer.h"

class TestWin : public Gtk::Window{
  public:

  GPlaneTestTile      pl1;
  GPlaneTestTileSlow  pl2;

  SimpleViewer *v;
  SimpleViewer *v1;
  MThreadViewer *v2;
  DThreadViewer *v3;

  TestWin(){
    v1 = new SimpleViewer(&pl1);
    v2 = new MThreadViewer(&pl2);
    v3 = new DThreadViewer(&pl2);
    v2->set_fast_plane(&pl1);
    v3->set_fast_plane(&pl1);
    signal_key_press_event().connect (sigc::mem_fun (this, &TestWin::on_key_press));
    v=(SimpleViewer *)v2;

    add(*v);
    set_default_size(640,480);
    show_all();
    std::cerr << "Viewer Test\n"
              << " '1' - SimpleViewer\n"
              << " '2' - MThreadViewer\n"
              << " '3' - DThreadViewer\n"
              << " 'R' - Refresh\n"
              << " 'Q' - Quit\n";
 }

  bool on_key_press(GdkEventKey * event) {
     std::cerr << "key_press: " << event->keyval << "\n";
     switch (event->keyval) {
       case 'r':
       case 'R':
         v->redraw();
         return true;
       case 'q':
       case 'Q':
         hide();
       case '1':
         change_viewer(v1);
         return true;
       case '2':
         change_viewer(v2);
         return true;
       case '3':
         change_viewer(v3);
         return true;
     }
     return false;
  }

  void change_viewer(SimpleViewer *v1){
    if (!v1) return;
    v->epoch++;
    v=v1;
    remove();
    add(*v);
    show_all();
    v->redraw();
  }
};



int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    TestWin       win;

    kit.run(win);
}
