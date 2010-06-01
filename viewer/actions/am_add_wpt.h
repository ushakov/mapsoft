#ifndef AM_ADD_WPT_H
#define AM_ADD_WPT_H

#include "action_mode.h"
#include "../generic_dialog.h"
#include "../mapview.h"

class AddWaypoint : public ActionMode {
public:
    AddWaypoint (Mapview * mapview_) : mapview(mapview_) { }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Add Waypoint";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() { }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) {
	std::cout << "ADDWPT: " << p << std::endl;

	LayerWPT * layer;
	current_layer = NULL;
	for (int i=0; i<mapview->wpt_layers.size(); i++){
          layer = dynamic_cast<LayerWPT *> (mapview->wpt_layers[i].get());
          if (mapview->workplane.get_layer_active(layer)) {
	      current_layer = layer;
	      break;
	  }
        }

        if (current_layer==0){
          boost::shared_ptr<geo_data> world (new geo_data);
          mapview->data.push_back(world);

          boost::shared_ptr<LayerWPT> wpt_layer(new LayerWPT(world.get()));
          wpt_layer->set_ref(mapview->reference);
          mapview->wpt_layers.push_back(wpt_layer);
          mapview->add_layer(wpt_layer.get(), 100, "wpt: new");
          current_layer = dynamic_cast<LayerWPT *>(wpt_layer.get());
        }

        g_map map = current_layer->get_ref();

        convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));
        g_waypoint wpt;
        wpt.x = p.x; wpt.y=p.y;
	cnv.frw(wpt);
	Options opt = wpt.to_options();

	mapview->gend.activate(get_name(), opt,
	  sigc::mem_fun(this, &AddWaypoint::on_result));
    }

private:
    Mapview       * mapview;
    LayerWPT      * current_layer;

    void on_result(int r, const Options & o) {
	if (r == 0) { // OK
          assert (current_layer);
	  g_waypoint wpt; 
          wpt.parse_from_options(o);
          if (current_layer->get_world()->wpts.size()==0) 
	    current_layer->get_world()->wpts.push_back(g_waypoint_list());
          current_layer->get_world()->wpts[0].push_back(wpt);
          mapview->workplane.refresh_layer(current_layer);
   	  std::cout << "ADDWPT: " << wpt.name << "\n";
	}
    }
};

#endif /* AM_ADD_WPT_H */
