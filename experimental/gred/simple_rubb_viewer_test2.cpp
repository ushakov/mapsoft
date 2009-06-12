#include <iostream>
#include "gplane.h"
#include "simple_viewer.h"
#include "rubber_viewer.h"


int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GPlaneTestGrid  p1;

    RubberViewer<SimpleViewer> viewer(&p1);

    viewer.rubber_add( 5, 5,  0,   5,15, 0);
    viewer.rubber_add( 5,15,  0,  15,15, 0);
    viewer.rubber_add(15,15,  0,  15, 5, 0);
    viewer.rubber_add(15, 5,  0,   5, 5, 0);

    viewer.rubber_add(10,10,  0,   0, 0, 3);

    viewer.rubber_add( 0,10,  1,   0, 0, 3);
    viewer.rubber_add(10, 0,  2,   0, 0, 3);
    viewer.rubber_add(10,10,  0,   0,10, 1);
    viewer.rubber_add(10,10,  0,  10, 0, 2);

    viewer.rubber_add(-5,-5,  3,  -5, 5, 3);
    viewer.rubber_add(-5, 5,  3,   5, 5, 3);
    viewer.rubber_add( 5, 5,  3,   5,-5, 3);
    viewer.rubber_add( 5,-5,  3,  -5,-5, 3);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
