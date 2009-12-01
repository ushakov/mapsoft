#ifndef AM_ADD_WPT_H
#define AM_ADD_WPT_H

#include "action_mode.h"
#include "../generic_dialog.h"
#include "../mapview.h"

class AddWaypoint : public ActionMode {
public:
    AddWaypoint (Mapview * state_) : state(state_) {
	gend = GenericDialog::get_instance();
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Add Waypoint";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {
	gend->deactivate();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p) {
	std::cout << "ADDWPT: " << p << std::endl;

	LayerWPT * layer;
	current_layer = NULL;
	for (int i=0; i<state->wpt_layers.size(); i++){
          layer = dynamic_cast<LayerWPT *> (state->wpt_layers[i].get());
          if (state->viewer->workplane.get_layer_active(layer)) {
	      current_layer = layer;
	      break;
	  }
        }
        if (current_layer == 0) return; // надо бы добавлять новый, но для этого нужен доступ
	                                // к layer_list и т.п.

        g_map map = current_layer->get_ref();

        convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));
        g_waypoint wpt;
        wpt.x = p.x; wpt.y=p.y;
	cnv.frw(wpt);
	Options opt = wpt.to_options();

	current_connection = gend->signal_result().connect(sigc::mem_fun(this, &AddWaypoint::on_result));
	gend->activate(get_name(), opt);
    }

private:
    Mapview       * state;
    GenericDialog * gend;
    LayerWPT      * current_layer;

    sigc::connection current_connection;

    void on_result(int r) {
	if (r == 0) { // OK
          assert (current_layer);
	  g_waypoint wpt; 
          wpt.parse_from_options(gend->get_options());
          if (current_layer->get_world()->wpts.size()==0) 
	    current_layer->get_world()->wpts.push_back(g_waypoint_list());
          current_layer->get_world()->wpts[0].push_back(wpt);
          state->viewer->workplane.refresh_layer(current_layer);
   	  std::cout << "ADDWPT: " << wpt.name << "\n";
	} else {
	  // do nothing
	}
        current_connection.disconnect();
    }
};

#endif /* AM_ADD_WPT_H */
