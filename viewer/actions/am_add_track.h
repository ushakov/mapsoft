#ifndef AM_ADD_TRACK_H
#define AM_ADD_TRACK_H

#include <sstream>

#include "action_mode.h"
#include "../generic_dialog.h"

class AddTrack : public ActionMode {
public:
    AddTrack (Mapview * mapview) : ActionMode(mapview) { }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Add Track";
    }

    // Activates this mode.
    virtual void activate() {
      new_track.clear();
    }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {
      activate();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) {


        /// find/create current_layer at the first click
	if (new_track.size() == 0){
          current_layer = NULL;
          LayerTRK * layer;
          for (int i=0; i<mapview->trk_layers.size(); i++){
            layer = dynamic_cast<LayerTRK *> (mapview->trk_layers[i].get());
            if (mapview->workplane.get_layer_active(layer)) {
              current_layer = layer;
              break;
            }
          }
          if (current_layer == NULL){
            boost::shared_ptr<geo_data> world (new geo_data);
            mapview->data.push_back(world);

            boost::shared_ptr<LayerTRK> trk_layer(new LayerTRK(world.get()));
            trk_layer->set_ref(mapview->reference);
            mapview->trk_layers.push_back(trk_layer);
            mapview->add_layer(trk_layer.get(), 200, "trk: new");
            current_layer = dynamic_cast<LayerTRK *>(trk_layer.get());
          }

 	  Options opt = new_track.to_options();
	  mapview->gend.activate(get_name(), opt, 
	    sigc::mem_fun(this, &AddTrack::on_result));
        }

	/// add or remove point
        if (state&Gdk::CONTROL_MASK){
	  std::cout << "remove track point" << std::endl;
          if (new_track.size()>0) new_track.pop_back();
          if (mapview->rubber.size()>0){
            mapview->rubber.pop();
          }
          if (mapview->rubber.size()>0){
            RubberSegment s = mapview->rubber.pop();
            s.flags |= RUBBFL_MOUSE_P2;
            s.p2=iPoint(0,0);
            mapview->rubber.add(s);
          }
        }
        else{
	  std::cout << "add track point: " << p << std::endl;
          if (mapview->rubber.size()>0){
            RubberSegment s = mapview->rubber.pop();
            s.flags &= ~RUBBFL_MOUSE;
            s.p2 = p;
            mapview->rubber.add(s);
          }
          mapview->rubber.add_diag(p);
          g_map map = current_layer->get_ref();
          convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));

          g_trackpoint pt;
          pt.x = p.x; pt.y=p.y;
  	  cnv.frw(pt);
 	  new_track.push_back(pt);
        }

	std::ostringstream st;
	st << "Creating new track... "
           << new_track.size() << " points, "
           << new_track.length()/1000 << " km";
	mapview->statusbar.push(st.str(),0);
    }

private:
    LayerTRK      * current_layer;

    g_track  new_track;

    void on_result(int r, const Options & o) {
       if (r == 0) { // OK
          assert (current_layer);
          new_track.parse_from_options(o);
         current_layer->get_world()->trks.push_back(new_track);
          mapview->workplane.refresh_layer(current_layer);
       }
       mapview->statusbar.push("",0);
       new_track.clear();
       mapview->rubber.clear();
    }

};

#endif /* AM_ADD_TRACK_H */
