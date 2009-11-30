#include <iostream>
#include "gobj.h"
#include "simple_viewer.h"
#include "rubber.h"


int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestTile  p1;

    SimpleViewer viewer(&p1);
    Rubber rubber(&viewer);

    iPoint p(20,20);
    rubber.rubber_add_src_sq(p, 3);
    rubber.rubber_add_dst_sq(3);
    rubber.rubber_add_rect(p);
    rubber.rubber_add_diag(p);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
