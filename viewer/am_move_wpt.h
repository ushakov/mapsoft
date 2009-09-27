#ifndef AM_MOVE_WPT_H
#define AM_MOVE_WPT_H

#include "action_mode.h"
#include "action_manager.h"
#include "mapview.h"

class MoveWaypoint : public ActionMode {
public:
    MoveWaypoint (MapviewState * state_, Viewer * viewer_) : state(state_), viewer(viewer_) {
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
        viewer->rubber.clear();
        mystate=0;
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p) {

        if (mystate==0){ // select point
          std::cout << " MOVEWPT: " << p << std::endl;
	  for (int i = 0; i < state->wpt_layers.size(); ++i) {
            current_layer = dynamic_cast<LayerWPT *> (state->wpt_layers[i].get());
            if (!viewer->workplane.get_layer_active(current_layer)) continue;
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_waypoint(p);
	    if (d.first >= 0) {
		std::cout << "MOVEWPT: found at " << current_layer << std::endl;
		current_wpt = &(current_layer->get_world()->wpts[d.first][d.second]);
                iPoint dp(2,2);
                viewer->rubber.add_line(p+iPoint(-2,-2),false, p+Point<int>( 2,-2),false);
                viewer->rubber.add_line(p+iPoint( 2,-2),false, p+Point<int>( 2, 2),false);
                viewer->rubber.add_line(p+iPoint( 2, 2),false, p+Point<int>(-2, 2),false);
                viewer->rubber.add_line(p+iPoint(-2, 2),false, p+Point<int>(-2,-2),false);

                viewer->rubber.add_line(iPoint(-2,-2),true, Point<int>( 2,-2),true);
                viewer->rubber.add_line(iPoint( 2,-2),true, Point<int>( 2, 2),true);
                viewer->rubber.add_line(iPoint( 2, 2),true, Point<int>(-2, 2),true);
                viewer->rubber.add_line(iPoint(-2, 2),true, Point<int>(-2,-2),true);

                viewer->rubber.add_line(p,false, iPoint(0,0),true);
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
          current_wpt->x = pt.x;
          current_wpt->y = pt.y;
          viewer->workplane.refresh_layer(current_layer);
	  mystate=0;
          viewer->rubber.clear();
          current_layer=0;
          current_wpt=0;
        }
    }

private:
    MapviewState * state;
    Viewer       * viewer;
    g_waypoint   * current_wpt;
    LayerWPT     * current_layer;
    int mystate; // 0 - select point, 1 - move point
};

#endif /* AM_MOVE_WPT_H */
