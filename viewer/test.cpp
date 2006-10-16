#include <gtkmm.h>

//#include <boost/shared_ptr.hpp>

//#include <io_new/io.h>
#define DEBUG_VIEWER
//#define DEBUG_GOOGLE

#include "viewer.h"
#include <point.h>
#include "workplane.h"
#include "layer_grid.h"
#include "layer_google1.h"
#include "layer_wait.h"
#include "layer_jpeg_simple.h"

bool
on_keypress ( GdkEventKey * event, Workplane * w, Viewer * v ) {
    std::cerr << "key: " << event->keyval << std::endl;
/*
    switch (event->keyval) {
    case 43:
    case 65451: // +
    {
	Point<int> orig = v->get_window_origin();
//	double scale = w->get_scale();
	std::cerr << "scale was: " << scale << "; orig was " << orig.x << "," << orig.y << std::endl;
//	w->set_scale(scale / 1.2);
	v->set_window_origin(PointRR (int (orig.x * 1.2), int (orig.y * 1.2)));
	return true;
    }
    case 45:
    case 65453: // -
    {
	PointRR orig = v->get_window_origin();
	double scale = w->get_scale();
	w->set_scale(scale * 1.2);
	v->set_window_origin(PointRR (orig.x / 1.2, orig.y / 1.2));
	return true;
    }
    }*/
    return false;
}


int
main(int argc, char **argv)
{
    Gtk::Main kit (argc, argv);

    Gtk::Window win (Gtk::WINDOW_TOPLEVEL);
    Workplane w(256,0);
    
    LayerGrid l1(50,50,0xFF000080);
    LayerWait l2;
//    LayerJpegSimple l3("/d2/1km/O36/O36-001.jpg");
    LayerGoogle1 l3("/e2/M/GOOGLE",6);

//    w.add_layer(&l1,100);
    w.add_layer(&l3,200);

    Viewer viewer (w, 256);

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
