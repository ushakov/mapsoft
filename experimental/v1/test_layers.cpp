#include <gtkmm.h>

#include <viewer/viewer.h>
#include "layer_cgrid.h"

int main(int argc, char **argv){
  Gtk::Main app (argc, argv);
  Gtk::Window win (Gtk::WINDOW_TOPLEVEL);
  win.set_default_size(640,480);

  LayerCGrid lcg(100);

  Viewer viewer;
  viewer.workplane.add_layer(&lcg, 1);

  viewer.workplane.get_image(Point<int>(-25,0));
  viewer.workplane.get_image(Point<int>(-25,1));

  win.add(viewer);
  win.show_all();
  app.run(win);
}

