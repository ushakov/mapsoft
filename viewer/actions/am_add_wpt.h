#ifndef AM_ADD_WPT_H
#define AM_ADD_WPT_H

#include "action_mode.h"
#include "../generic_dialog.h"

class AddWaypoint : public ActionMode {
public:
    AddWaypoint (Mapview * mapview) : ActionMode(mapview) { }

    // Returns name of the mode as string.
    std::string get_name() {
	return "Add Waypoint";
    }

    // Activates this mode.
    void activate() { }

    // Abandons any action in progress and deactivates mode.
    void abort() { }

    // Sends user click. Coordinates are in workplane's discrete system.
    void handle_click(iPoint p, const Gdk::ModifierType & state) {
         if (!mapview->have_reference){
           mapview->statusbar.push("No geo-reference", 0);
           return;
         }
        g_map map = mapview->reference;
        convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));
        g_waypoint wpt;
        wpt.x = p.x; wpt.y=p.y;
	cnv.frw(wpt);
	Options opt = wpt.to_options();

	mapview->gend.activate(get_name(), opt,
	  sigc::mem_fun(this, &AddWaypoint::on_result));
        mapview->rubber.clear();
        mapview->rubber.add_src_mark(p);
    }

private:

    void on_result(int r, const Options & o) {
        mapview->rubber.clear();
	if (r == 0) { // OK

	  g_waypoint wpt;
          wpt.parse_from_options(o);

          // try to find active wpt layer
          for (int i=0; i<mapview->wpt_layers.size(); i++){
           LayerWPT * layer = dynamic_cast<LayerWPT *>
              (mapview->wpt_layers[i].get());
            if (mapview->workplane.get_layer_active(layer)) {
                layer->get_world()->wpts[0].push_back(wpt);
                mapview->workplane.refresh_layer(layer);
	        return;
            }
          }

          // if there is no active wpt layer
          boost::shared_ptr<geo_data> world(new geo_data);
          g_waypoint_list wpts;
          wpts.push_back(wpt);
          world->wpts.push_back(wpts);
          mapview->add_world(world, "new", false);
	}
    }
};

#endif /* AM_ADD_WPT_H */
