#ifndef AM_ADD_TRACK_H
#define AM_ADD_TRACK_H

#include <sstream>

#include "action_mode.h"
#include "../generic_dialog.h"
#include "../mapview.h"

class AddTrack : public ActionMode {
public:
    AddTrack (Mapview * state_) : state(state_) { }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Add Track";
    }

    // Activates this mode.
    virtual void activate() {
      new_track.clear();
    }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() { }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p) {
	std::cout << "ADDTRACK: " << p << std::endl;

	if (new_track.size() == 0){ // первое тыканье
          // найдем layer, в который можно запихать трек
          current_layer = NULL;
          LayerTRK * layer;
          current_layer = NULL;
          for (int i=0; i<state->trk_layers.size(); i++){
            layer = dynamic_cast<LayerTRK *> (state->trk_layers[i].get());
            if (state->viewer.workplane.get_layer_active(layer)) {
              current_layer = layer;
              break;
            }
          }
          if (current_layer == NULL){
            boost::shared_ptr<geo_data> world (new geo_data);
            state->data.push_back(world);

            boost::shared_ptr<LayerTRK> trk_layer(new LayerTRK(world.get()));
            trk_layer->set_ref(state->reference);
            state->trk_layers.push_back(trk_layer);
            state->add_layer(trk_layer.get(), 200, "trk: new");
            current_layer = dynamic_cast<LayerTRK *>(trk_layer.get());
          }

 	  Options opt = new_track.to_options();
	  state->gend.activate(get_name(), opt, 
	    sigc::mem_fun(this, &AddTrack::on_result));
        }

        g_map map = current_layer->get_ref();
        convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));

        g_trackpoint pt;
        pt.x = p.x; pt.y=p.y;
	cnv.frw(pt);

	new_track.push_back(pt);
	std::ostringstream st;
	st << "Creating new track... "
           << new_track.size() << " points, "
           << new_track.length()/1000 << " km";
	state->statusbar.push(st.str(),0);
        state->rubber.clear();
	for (int i = 0; i<new_track.size()-1; i++){
	  g_point p1 = new_track[i];
	  g_point p2 = new_track[i+1];
          cnv.bck(p1); cnv.bck(p2);
	  state->rubber.add(p1,p2, RUBBFL_PLANE);
        }
	state->rubber.add_diag(p);

    }

private:
    Mapview       * state;
    LayerTRK      * current_layer;

    g_track  new_track;

    void on_result(int r, const Options & o) {
       if (r == 0) { // OK
          assert (current_layer);
          new_track.parse_from_options(o);
         current_layer->get_world()->trks.push_back(new_track);
          state->viewer.workplane.refresh_layer(current_layer);
       }
       state->statusbar.push("",0);
       new_track.clear();
       state->rubber.clear();
    }

};

#endif /* AM_ADD_TRACK_H */
