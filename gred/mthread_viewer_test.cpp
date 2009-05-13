#include <iostream>
#include "mthread_viewer.h"


int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GPlane_test1  pl1;
    GPlane_test2  pl2;

    ThreadViewer viewer(&pl1, &pl2);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
