#include <iostream>
#include "mthread_viewer.h"


int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GPlaneTestTile      pl1;
    GPlaneTestTileSlow  pl2;

    MThreadViewer viewer(&pl1, &pl2);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
