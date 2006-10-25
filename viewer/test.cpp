#include <gtkmm.h>

//#include <boost/shared_ptr.hpp>

//#include <io_new/io.h>
//#define DEBUG_VIEWER
//#define DEBUG_GOOGLE

#include "viewer.h"
#include <point.h>
#include "workplane.h"
#include "layer_grid.h"
#include "layer_google1.h"
#include "layer_wait.h"
#include "layer_jpeg_simple.h"
#include "layer_geodata_old.h"

int sc = 1;

const std::string google_dir = "/e2/M/GOOGLE";
const std::string data_file  = "./track.plt";

LayerGoogle1 gl(google_dir,sc);
LayerGeodataOld dl(data_file, sc);

bool
on_keypress ( GdkEventKey * event, Workplane * w, Viewer * v ) {

    switch (event->keyval) {
    case 105:
    case 73: // i
    {
	if (sc>=18) break;
	sc++;
        gl = LayerGoogle1(google_dir,sc);
        dl.set_scale(sc);
	Point<int> orig = v->get_window_origin() + v->get_window_size()/2;
	std::cerr << "google scale: " << sc << " scale: " 
                  << v->scale_nom() << ":" 
                  << v->scale_denom() <<  std::endl;
	v->set_window_origin(orig*2 - v->get_window_size()/2);
        v->clear_cache();
	return true;
    }
    case 111:
    case 79: // -
    {
	if (sc<=1) break;
	sc--;
	gl = LayerGoogle1(google_dir,sc);
        dl.set_scale(sc);
	std::cerr << "google scale: " << sc << " scale: " 
                  << v->scale_nom() << ":" 
                  << v->scale_denom() <<  std::endl;
	Point<int> orig = v->get_window_origin() + v->get_window_size()/2;
	v->set_window_origin(orig/2 - v->get_window_size()/2);
        v->clear_cache();
	return true;
    }
    case 43:                                                                           
    case 65451: // +                                                                   
    {                                                                                  
      v->scale_inc();                                                                     
	std::cerr << "google scale: " << sc << " scale: " 
                  << v->scale_nom() << ":" 
                  << v->scale_denom() <<  std::endl;
      return true;                                                                     
    }                                                                                  
    case 45:                                                                           
    case 65453: // -                                                                   
    {                                                                                  
      v->scale_dec();                                                                     
	std::cerr << "google scale: " << sc << " scale: " 
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

    Workplane w(256,0);
    
    LayerGrid l1(200.5,200.5,0xFF000080);
//    LayerWait l2;
//    LayerJpegSimple l3("/d2/1km/O36/O36-001.jpg");
//    LayerGoogle1 l3("/e2/M/GOOGLE",6);

    w.add_layer(&l1,100);
//    w.add_layer(&l2,100);
    w.add_layer(&gl,200);
    w.add_layer(&dl,50);

    Viewer viewer (w);
//    viewer.set_window_origin(dl.range().TLC());

//    geo_data world;
//    options opts;
//    std::string filename (argv[1]);
//    std::cerr << "File: " << filename << std::endl;
//    io::in (filename, world, opts);
//    std::vector<boost::shared_ptr<MapLayer> > maps;

//    for (size_t i = 0; i != world.maps.size(); ++i)
//    {
//	maps.push_back (boost::shared_ptr<MapLayer> (new MapLayer (world.maps[i])));
//    }
    
//    for (size_t i = 0; i != maps.size(); ++i)
//    {
//	w.add_layer (maps[i].get(), 100);
//	w.set_scale (maps[i]->gmap.m_per_pix);
//	w.set_lon0 (maps[i]->gmap.getlon0());
//	std::cerr << "border tmerc: " << maps[i]->gmap.border_tmerc[0].x << "," << maps[i]->gmap.border_tmerc[0].y << std::endl;
//	PointRR plane = maps[i]->tmerc2plane (maps[i]->gmap.border_tmerc[0]);
//	viewer.set_window_origin (plane);
//    }
    win.add (viewer);
    win.signal_key_press_event().connect (sigc::bind<1> (sigc::bind<1> (sigc::ptr_fun (&on_keypress), &w), &viewer));

//    viewer.set_window_origin ((int) (203), (int) (-3068));
// 203, -3068
// 20300 306800    
    win.show_all();
    kit.run(win);
}
