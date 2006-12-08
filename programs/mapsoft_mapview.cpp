#include <gtkmm.h>

//#define DEBUG_LAYER_R

#include "viewer.h"
#include <point.h>
#include "workplane.h"
#include "layer_grid.h"
#include "layer_rh.h"
#include "layer_wait.h"

#include "../geo_io/io.h"
#include "layer_geodata1.h"



bool
on_keypress ( GdkEventKey * event, Workplane * w, Viewer * v ) {

    switch (event->keyval) {
    case 43:                                                                           
    case 65451: // +                                                                   
    {                                                                                  
      v->scale_inc();                                                                     
	std::cerr << "scale: " 
                  << v->scale_nom() << ":" 
                  << v->scale_denom() <<  std::endl;
      return true;                                                                     
    }                                                                                  
    case 45:                                                                           
    case 65453: // -                                                                   
    {                                                                                  
      v->scale_dec();                                                                     
	std::cerr << "scale: " 
                  << v->scale_nom() << ":" 
                  << v->scale_denom() <<  std::endl;
      return true;                                                                     
    }                                                                                  
    }
    return false;
}


int
main(int argc, char **argv)
{
    Gtk::Main kit (argc, argv);

    Gtk::Window win (Gtk::WINDOW_TOPLEVEL);



    if (argc <2) {
      std::cerr << "Usage: mapsoft_imgview <geo files>\n";
      exit(0);
    }
    geo_data world;

    //чтение файлов из командной строки:
    for(int i=1;i<argc;i++){
      io::in(std::string(argv[i]), world, Options());
    }
    if (world.maps.size()==0){
      std::cerr << "You must give me a map!\n";
      exit(0);
    }

    LayerRH l(argv[1]);
    LayerGeodata1 dl(world.maps[0], world);

    Workplane w(256,0);

    w.add_layer(&l,100);
    w.add_layer(&dl,100);

    Viewer viewer (w);
    win.add (viewer);
    win.signal_key_press_event().connect (sigc::bind<1> (sigc::bind<1> (sigc::ptr_fun (&on_keypress), &w), &viewer));

    win.show_all();
    kit.run(win);
}
