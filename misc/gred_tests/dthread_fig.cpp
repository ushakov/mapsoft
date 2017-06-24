#include <iostream>
#include <fstream>
#include "gred/dthread_viewer.h"
#include "gobj_fig.h"


int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;


    if (argc!=2){
      std::cerr << "usage: dthread_line <fig file>\n";
      exit(1);
    }

    fig::fig_world F;
    fig::read(argv[1], F);

    std::cerr << F.size() << " objects...\n";

    GObjFIG O(F);
    DThreadViewer viewer(&O);
    viewer.set_bgcolor(0xfff0f0f0);
    viewer.set_origin(O.range().TLC());
    viewer.set_scale(1);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);

}
