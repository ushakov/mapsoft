#include "gred.h"

#include "gobj_test_grid.h"
#include "action_test_circ.h"

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestGrid  p1(150000);

    DThreadViewer viewer(&p1);
    Rubber  rubber(&viewer);
    Actions actions(&viewer);
    ActionTestCirc A1(&rubber);
    actions.add(&A1);
    actions.select("TestCirc");

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
