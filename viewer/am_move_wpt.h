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
    virtual void abort() { }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(Point<int> p) {

        if (mystate==0){
          std::cout << " MOVEWPT: " << p << std::endl;
	  for (int i = 0; i < state->data_layers.size(); ++i) {
            current_layer = dynamic_cast<LayerGeoData *> (state->data_layers[i].get());
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_waypoint(p);
	    if (d.first >= 0) {
		std::cout << "MOVEWPT: found at " << current_layer << std::endl;
		current_wpt = &(current_layer->get_world()->wpts[d.first][d.second]);
                mystate=1;
            }
	  }
	} else {
          assert (current_layer);
          g_map map = current_layer->get_ref();
          convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"),Options());
          g_point pt(p.x, p.y);
          cnv.frw(pt);
          current_wpt->x = pt.x;
          current_wpt->y = pt.y;
          state->workplane->mark_level_dirty(current_layer);
	  mystate=0;
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
