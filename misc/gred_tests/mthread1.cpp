#include <iostream>
#include "gred/mthread_viewer.h"
#include "gobj_test_tile.h"

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestTile  o2(true);

    MThreadViewer viewer(&o2);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
