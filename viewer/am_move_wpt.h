#ifndef AM_MOVE_WPT_H
#define AM_MOVE_WPT_H

#include <viewer/action_mode.h>
#include <viewer/action_manager.h>
#include <programs/mapview.h>

class MoveWaypoint : public ActionMode {
public:
    MoveWaypoint (MapviewState * state_) : state(state_) {
	current_wpt = 0;
        current_layer = 0;
        mystate = 0;
    }

    // Returns name of the mode as string.
    virtual std::string get_name() { return "Move Waypoint"; }

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
          std::cout << " MOVEWPT: " << p << std::endl;
	  for (int i = 0; i < state->data_layers.size(); ++i) {
            current_layer = dynamic_cast<LayerGeoData *> (state->data_layers[i].get());
            if (!state->workplane->get_layer_active(current_layer)) continue;
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_waypoint(p);
	    if (d.first >= 0) {
		std::cout << "MOVEWPT: found at " << current_layer << std::endl;
		current_wpt = &(current_layer->get_world()->wpts[d.first][d.second]);
                Point<int> dp(2,2);
                state->rubber->add_line(RubberPoint(p+Point<int>(-2,-2),1), RubberPoint(p+Point<int>( 2,-2),1));
                state->rubber->add_line(RubberPoint(p+Point<int>( 2,-2),1), RubberPoint(p+Point<int>( 2, 2),1));
                state->rubber->add_line(RubberPoint(p+Point<int>( 2, 2),1), RubberPoint(p+Point<int>(-2, 2),1));
                state->rubber->add_line(RubberPoint(p+Point<int>(-2, 2),1), RubberPoint(p+Point<int>(-2,-2),1));

                state->rubber->add_line(RubberPoint(Point<int>(-2,-2),0), RubberPoint(Point<int>( 2,-2),0));
                state->rubber->add_line(RubberPoint(Point<int>( 2,-2),0), RubberPoint(Point<int>( 2, 2),0));
                state->rubber->add_line(RubberPoint(Point<int>( 2, 2),0), RubberPoint(Point<int>(-2, 2),0));
                state->rubber->add_line(RubberPoint(Point<int>(-2, 2),0), RubberPoint(Point<int>(-2,-2),0));

                state->rubber->add_line(RubberPoint(p,1), RubberPoint(Point<int>(0,0),0));
                mystate=1;
		break;
            }
	  }
	} else { // move point
          assert (current_layer);
          g_map map = current_layer->get_ref();
          convs::map2pt cnv(map, "wgs84", "lonlat");
          g_point pt(p.x, p.y);
          cnv.frw(pt);
          current_wpt->x = pt.x;
          current_wpt->y = pt.y;
          state->workplane->refresh_layer(current_layer);
	  mystate=0;
          state->rubber->clear();
          current_layer=0;
          current_wpt=0;
        }
    }

private:
    MapviewState * state;
    g_waypoint * current_wpt;
    LayerGeoData * current_layer;
    int mystate; // 0 - select point, 1 - move point
};

#endif /* AM_MOVE_WPT_H */
