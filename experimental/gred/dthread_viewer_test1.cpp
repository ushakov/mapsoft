#include <iostream>
#include "dthread_viewer.h"
#include "gobj.h"

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestTile      pl1;
    GObjTestTileSlow  pl2;

    DThreadViewer viewer(&pl2);
    viewer.set_fast_obj(&pl1);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
