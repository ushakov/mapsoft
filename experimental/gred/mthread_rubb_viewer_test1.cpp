#include <iostream>
#include "gplane.h"
#include "mthread_viewer.h"
#include "rubber_viewer.h"


int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GPlaneTestGridSlow  p1;

    RubberViewer<MThreadViewer> viewer(&p1);

    iPoint p(20,20);
    viewer.rubber_add_src_sq(p, 3);
    viewer.rubber_add_dst_sq(3);
    viewer.rubber_add_rect(p);
    viewer.rubber_add_diag(p);

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
