#ifndef AM_MOVE_WPT_H
#define AM_MOVE_WPT_H

#include "action_mode.h"
#include "../mapview.h"

class MoveWaypoint : public ActionMode {
public:
    MoveWaypoint (Mapview * mapview_) : mapview(mapview_) {
	current_wpt = 0;
        current_layer = 0;
        mystate = 0;
    }

    // Returns name of the mode as string.
    virtual std::string get_name() { return "Move Waypoint"; }

    // Activates this mode.
    virtual void activate() {
      mystate=0;
    }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {
      activate();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) {

        if (mystate==0){ // select point
          std::cout << " MOVEWPT: " << p << std::endl;
	  for (int i = 0; i < mapview->wpt_layers.size(); ++i) {
            current_layer = dynamic_cast<LayerWPT *> (mapview->wpt_layers[i].get());
            if (!mapview->workplane.get_layer_active(current_layer)) continue;
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_waypoint(p);
	    if (d.first >= 0) {
		std::cout << "MOVEWPT: found at " << current_layer << std::endl;
		current_wpt = &(current_layer->get_world()->wpts[d.first][d.second]);
		mapview->rubber.add_src_sq(p, 2);
		mapview->rubber.add_dst_sq(2);
                mapview->rubber.add_diag(p);
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
          mapview->workplane.refresh_layer(current_layer);
	  mystate=0;
          mapview->rubber.clear();
          current_layer=0;
          current_wpt=0;
        }
    }

private:
    Mapview      * mapview;
    g_waypoint   * current_wpt;
    LayerWPT     * current_layer;
    int mystate; // 0 - select point, 1 - move point
};

#endif /* AM_MOVE_WPT_H */
