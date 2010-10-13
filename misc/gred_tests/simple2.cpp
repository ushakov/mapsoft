#include <iostream>
#include "simple_viewer.h"
#include "gobj_test_grid.h"

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestGrid  pl;

    SimpleViewer viewer(&pl);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
