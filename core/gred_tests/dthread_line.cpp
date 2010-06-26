#include <iostream>
#include <fstream>
#include "dthread_viewer.h"
#include "gobj_line.h"

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;


    iLine L;
    iPoint p;
    std::ifstream f("dthread_line.txt");
    while (!f.eof()) {f >> p; L.push_back(p);}


    GObjLine      O(L, 0xFF00FF00, 3);
    DThreadViewer viewer(&O);

std::cerr << O.range();

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
