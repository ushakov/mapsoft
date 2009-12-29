#ifndef AM_MAKE_TILES_H
#define AM_MAKE_TILES_H

#include <sstream>

#include "action_mode.h"
#include "../generic_dialog.h"
#include "../mapview.h"

class MakeTiles : public ActionMode {
public:
    MakeTiles (Mapview * state_) : state(state_) {
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Make Tiles";
    }

    // Activates this mode.
    virtual void activate() {
      have_points = 0;
    }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() { }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p) {
	std::cout << "MAKETILES: " << p << " points: " << have_points << std::endl;
	if (have_points == 0) {
	    g_point geo = p;
	    if (!get_geo_point(geo)) return;
	    one = geo;
	    have_points = 1;
	    // make rubber
	    state->rubber.clear();
	    state->rubber.add_rect(p);
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
	    state->rubber.clear();
	    state->gend.activate(get_name(), opt,
	      sigc::mem_fun(this, &MakeTiles::on_result));
	}
    }

    bool get_geo_point(g_point& p) {
	LayerGeoMap * layer;
	current_layer = NULL;
	for (int i = 0; i < state->map_layers.size(); i++){
	    layer = state->map_layers[i].get();
	    if (state->viewer.workplane.get_layer_active(layer)) {
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
    LayerGeoMap   * current_layer;
    int have_points;

    g_point one, two;

    void on_result(int r, const Options & o) {
	have_points = 0;
        if (r == 0) { // OK
	  std::string filename = o.get("dirname", std::string("tiles"));
	  filename += ".tiles";
	  tiles::write_file(filename.c_str(), *current_layer->get_world(), o);
	}
    }
};

#endif /* AM_MAKE_TILES_H */
