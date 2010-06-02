#include <iostream>
#include "mthread_viewer.h"
#include "gobj_test_tile.h"

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestTile      o1;
    GObjTestTileSlow  o2;

    MThreadViewer viewer(&o2);
    viewer.set_fast_obj(&o1);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
