#ifndef AM_MOVE_TPT_H
#define AM_MOVE_TPT_H

#include <viewer/action_mode.h>
#include <viewer/action_manager.h>
#include <programs/mapview.h>

class MoveTrackpoint : public ActionMode {
public:
    MoveTrackpoint (MapviewState * state_) : state(state_) {
	current_tpt = 0;
        current_layer = 0;
        mystate = 0;
    }

    // Returns name of the mode as string.
    virtual std::string get_name() { return "Move Trackpoint"; }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() { 
        state->rubber->clear();
        mystate=0;
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(Point<int> p) {

        if (mystate==0){ // select point
          std::cout << " MOVETPT: " << p << std::endl;
	  for (int i = 0; i < state->data_layers.size(); ++i) {
            current_layer = dynamic_cast<LayerGeoData *> (state->data_layers[i].get());
	    
            if (!state->workplane->get_layer_active(current_layer)) continue;
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_trackpoint(p);
	    if (d.first >= 0) {
		std::cout << "MOVETPT: found at " << current_layer << std::endl;
                g_map map = current_layer->get_ref();
                convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));

		current_tpt = &(current_layer->get_world()->trks[d.first][d.second]);
		if ((d.second > 0)&&(!current_layer->get_world()->trks[d.first][d.second].start)){
  		  g_point p1 = current_layer->get_world()->trks[d.first][d.second-1];
		  cnv.bck(p1);
                  state->rubber->add_line(p1,false, Point<int>(0,0),true);
		}
		if ((d.second < current_layer->get_world()->trks[d.first].size()-1)&&
		    (!current_layer->get_world()->trks[d.first][d.second+1].start)){
  		  g_point p1 = current_layer->get_world()->trks[d.first][d.second+1];
		  cnv.bck(p1);
                  state->rubber->add_line(p1,false, Point<int>(0,0),true);
		}
		if (state->rubber->size()==0)
                  state->rubber->add_line(p,false, Point<int>(0,0),true);
                mystate=1;
		break;
            }
	  }
	} else { // move point
          assert (current_layer);
          g_map map = current_layer->get_ref();
          convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));
          g_point pt(p.x, p.y);
          cnv.frw(pt);
          current_tpt->x = pt.x;
          current_tpt->y = pt.y;
          state->workplane->refresh_layer(current_layer);
	  mystate=0;
          state->rubber->clear();
          current_layer=0;
          current_tpt=0;
        }
    }

private:
    MapviewState * state;
    g_trackpoint * current_tpt;
    LayerGeoData * current_layer;
    int mystate; // 0 - select point, 1 - move point
};

#endif /* AM_MOVE_TPT_H */
