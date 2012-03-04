#include <gtkmm.h>

//#include <boost/shared_ptr.hpp>

//#include <io_new/io.h>
//#define DEBUG_VIEWER
//#define DEBUG_GOOGLE
//#define DEBUG_LAYER_GEOMAP
//#define DEBUG_LAYER_GEODATA
//#define DEBUG_JPEG
//#define DEBUG_MAPVIEW

#include "mapview.h"

#include "geo_io/io.h"

int
main(int argc, char **argv)
{
    Gtk::Main kit (argc, argv);

    Mapview mapview;

    if ((argc==2) && (io::testext(argv[1], ".xml"))){
      mapview.load_file(argv[1]);
    }
    else {
      std::list<std::string> files(argv+1,argv+argc);
      mapview.add_files(files);
    }

    kit.run(mapview);
}

