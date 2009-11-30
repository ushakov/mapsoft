#include "gobj_test_grid.h"
#include "dthread_viewer.h"
#include "actions.h"
#include "rubber.h"

#include "action_test_box.h"
#include "action_test_line.h"

int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GObjTestGrid  p1(150000);

    DThreadViewer viewer(&p1);
    Rubber  rubber(&viewer);
    Actions actions(&viewer);

    ActionTestBox  A1(&rubber);
    ActionTestLine A2(&rubber);
    actions.add(&A1);
    actions.add(&A2);
    actions.select("TestLine");

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
