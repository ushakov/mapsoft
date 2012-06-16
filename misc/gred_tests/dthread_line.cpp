#include <iostream>
#include <fstream>
#include "gred/dthread_viewer.h"
#include "gobj_line.h"


int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;


    if (argc!=2){
      std::cerr << "usage: dthread_line <data file>\n";
      exit(1);
    }
    std::ifstream f(argv[1]);

    iMultiLine L;
    iPoint p;
    int n=0;

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
        n+=Li.size();
        Li.clear();
      }
    }
    std::cerr << L.size() << " lines, " << n << " points...\n";

    GObjLine O(L, 0xFF00FF00, 2);
    DThreadViewer viewer(&O);
    viewer.set_origin(O.range().TLC());

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);

}
