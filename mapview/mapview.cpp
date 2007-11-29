#include "mapview.h"

int main(int argc, char **argv){
    Gtk::Main kit (argc, argv);
    Mapview mapview;

//    for (int i = 1; i < argc; ++i) {
//        mapview.load_file(argv[i]);
//    }
    kit.run(mapview);
}
