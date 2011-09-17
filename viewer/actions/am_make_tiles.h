#ifndef AM_MAKE_TILES_H
#define AM_MAKE_TILES_H

#include <sstream>

#include "action_mode.h"
#include "../generic_dialog.h"

class MakeTiles : public ActionMode {
public:
    MakeTiles (Mapview * mapview) : ActionMode(mapview) {
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
    virtual void abort() { 
      activate();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) {
	if (have_points == 0) {
	    one = p;
	    have_points = 1;
	    mapview->rubber.clear();
	    mapview->rubber.add_rect(p);
	} else if (have_points == 1) {
	    two = p;
	    have_points = 2;
	    Options opt;
	    dRect bb(one, two);
	    opt.put("geom", bb);
	    opt.put("google", 13);
	    opt.put("dirname", "tiles");
	    mapview->rubber.clear();
	    mapview->gend.activate(get_name(), opt,
	      sigc::mem_fun(this, &MakeTiles::on_result));
	}
    }

private:
    int have_points;

    dPoint one, two;

    void on_result(int r, const Options & o) {
      have_points = 0;
      if (r != 0) return;

      LayerGeoMap * layer = find_map_layer();
      if (!layer) return;

      convs::map2pt cnv(layer->get_cnv());
      cnv.frw(one);
      cnv.frw(two);

/// !!!! one, two not used!
      std::string filename =
        o.get("dirname", std::string("tiles"));
      filename += ".tiles";
      geo_data world;
      world.maps.push_back(*layer->get_data());
      tiles::write_file(filename.c_str(), world, o);
    }
};

#endif /* AM_MAKE_TILES_H */
