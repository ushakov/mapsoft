#include <iostream>
#include <fstream>
#include "dthread_viewer.h"
#include "gobj_line.h"


int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;


    iMultiLine L;
    iPoint p;
    std::ifstream f("dthread_line.txt");

    while (f) {
      iLine Li;
      std::string s;
      getline(f, s);

      std::istringstream sf(s);
      while (sf) {
        if (sf >> p) Li.push_back(p);
      }
      if (Li.size()>0){ 
        L.push_back(Li);
        Li.clear();
      }
    }

    GObjLine O(L, 0xFF00FF00, 2);
    DThreadViewer viewer(&O);
    viewer.set_origin(O.range().TLC());
    viewer.set_scale(1);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);

}
