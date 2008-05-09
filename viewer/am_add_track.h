#ifndef AM_ADD_TRACK_H
#define AM_ADD_TRACK_H

#include <viewer/action_mode.h>
#include <viewer/action_manager.h>
#include <viewer/generic_dialog.h>
#include <programs/mapview.h>

class AddTrack : public ActionMode {
public:
    AddTrack (MapviewState * state_) : state(state_) {   
	gend = GenericDialog::get_instance();
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Add Track";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {
	state->rubber->clear();
	new_track = g_track();
	gend->deactivate();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(Point<int> p) {
	std::cout << "ADDTRACK: " << p << std::endl;

	if (new_track.size() == 0){ // первое тыканье
          // найдем layer, в который можно запихать трек
	  for (int i=0; i<state->data_layers.size(); i++){
            current_layer = dynamic_cast<LayerGeoData *> (state->data_layers[i].get());
            if (!state->workplane->get_layer_active(current_layer)) continue;
	    break;
          }
          if (current_layer==0) return; // надо бы добавлять новый, но для этого нужен доступ
                                        // к layer_list и т.п.

 	  Options opt = new_track.to_options();
  	  current_connection = gend->signal_result().connect(sigc::mem_fun(this, &AddTrack::on_result));
	  gend->activate(get_name(), opt);
        }

        g_map map = current_layer->get_ref();
        convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));

        g_trackpoint pt;
        pt.x = p.x; pt.y=p.y;
	cnv.frw(pt);

	new_track.push_back(pt);
        state->rubber->clear();
	for (int i = 0; i<new_track.size()-1; i++){
	  g_point p1 = new_track[i];
	  g_point p2 = new_track[i+1];
          cnv.bck(p1); cnv.bck(p2);
  	  state->rubber->add_line(RubberPoint(Point<int>(p1),1), RubberPoint(Point<int>(p2),1));
        }
  	state->rubber->add_line(RubberPoint(p,1), RubberPoint(Point<int>(0,0),0));

    }

private:
    MapviewState * state;
    GenericDialog * gend;
    LayerGeoData * current_layer;

    g_track  new_track;

    sigc::connection current_connection;

    void on_result(int r) {
	if (r == 0) { // OK
          assert (current_layer);
          new_track.parse_from_options(gend->get_options());
	  current_layer->get_world()->trks.push_back(new_track);
          state->workplane->refresh_layer(current_layer);
	}
	new_track.clear();
	state->rubber->clear();
        current_connection.disconnect();
    }
};

#endif /* AM_ADD_TRACK_H */
