#include <gtkmm.h>
#include "dialogs/print.h"

int
main(int argc, char **argv){
  iImage img(200,200,0xFF0000FF);

  for (int i=10;i<100; i++){
    for (int j=10;j<100; j++){
      img.set(i,j, 0xFFFFFFFF);
    }
  }

  Gtk::Main kit (argc, argv);

//    Gtk::Window w;
//    w.show_all();
//    kit.run(w);

  exit (print_image(img));
}


