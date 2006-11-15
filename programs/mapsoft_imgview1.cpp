#include <gtkmm.h>


#define DEBUG_LAYER_R

#include "viewer.h"
#include <point.h>
#include "workplane.h"
#include "layer_grid.h"
#include "layer_rh.h"
#include "layer_wait.h"


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
      std::cerr << "Usage: mapsoft_imgview <image file>\n";
      exit(0);
    }
    
    LayerRH l(argv[1]);
    Workplane w(256,0);

    w.add_layer(&l,100);

    Viewer viewer (w);
    win.add (viewer);
    win.signal_key_press_event().connect (sigc::bind<1> (sigc::bind<1> (sigc::ptr_fun (&on_keypress), &w), &viewer));

    win.show_all();
    kit.run(win);
}
