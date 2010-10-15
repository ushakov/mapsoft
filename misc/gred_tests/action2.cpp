#include "gred/gred.h"

#include "gobj_test_grid.h"
#include "action_test_box.h"

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestGrid  p1(150000);

    DThreadViewer viewer(&p1);
    Rubber  rubber(&viewer);
    ActionManager AM(&viewer);
    ActionTestBox A1(&rubber);
    AM.add(&A1);
    AM.select(A1.get_name());

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
