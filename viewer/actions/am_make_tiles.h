#ifndef AM_MAKE_TILES_H
#define AM_MAKE_TILES_H

#include <sstream>

#include "action_mode.h"
#include "../generic_dialog.h"
#include "../mapview.h"

class MakeTiles : public ActionMode {
public:
    MakeTiles (Mapview * state_) : state(state_) {
      	gend = GenericDialog::get_instance();
	have_points = 0;
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Make Tiles";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {
	gend->deactivate();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p) {
	std::cout << "MAKETILES: " << p << " points: " << have_points << std::endl;
	if (have_points == 0) {
	    g_point geo = p;
	    if (!get_geo_point(geo)) return;
	    one = geo;
	    have_points = 1;
	    // make rubber
	    state->viewer->rubber.clear();
	    state->viewer->rubber.add_rect(p);
	} else if (have_points == 1) {
	    g_point geo = p;
	    if (!get_geo_point(geo)) return;
	    two = geo;
	    have_points = 2;
	    Options opt;
	    dRect bb(one, two);
	    opt.put("geom", bb);
	    opt.put("google", 17);
	    opt.put("dirname", "tiles");
	    current_connection = gend->signal_result().connect(sigc::mem_fun(this, &MakeTiles::on_result));
	    gend->activate(get_name(), opt);
	}
    }

    bool get_geo_point(g_point& p) {
	LayerGeoMap * layer;
	current_layer = NULL;
	for (int i = 0; i < state->map_layers.size(); i++){
	    layer = state->map_layers[i].get();
	    if (state->viewer->workplane.get_layer_active(layer)) {
		current_layer = layer;
		break;
	    }
        }
        if (current_layer == 0) return false;
        g_map map = current_layer->get_ref();
        convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));
	cnv.frw(p);
	return true;
    }

private:
    Mapview       * state;
    GenericDialog * gend;
    LayerGeoMap   * current_layer;
    sigc::connection current_connection;
    int have_points;

    g_point one, two;

    void on_result(int r) {
	Options opt = gend->get_options();
	std::string filename = opt.get("dirname", std::string("tiles"));
	filename += ".tiles";
	tiles::write_file(filename.c_str(), *current_layer->get_world(), opt);
    }
};

#endif /* AM_MAKE_TILES_H */
