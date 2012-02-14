#include <gtkmm.h>
#include "widgets/page_box.h"

int
main(int argc, char **argv)
{
    Gtk::Main kit (argc, argv);

    Gtk::Window w;
    PageBox page;
    w.add(page);
    w.show_all();
    kit.run(w);
}

