#ifndef AM_MOVE_TPT_H
#define AM_MOVE_TPT_H

#include "action_mode.h"
#include "../mapview.h"

class MoveTrackpoint : public ActionMode {
public:
    MoveTrackpoint (Mapview * mapview_) : mapview(mapview_) {
	current_tpt = 0;
        current_layer = 0;
        mystate = 0;
    }

    // Returns name of the mode as string.
    virtual std::string get_name() { return "Move Trackpoint"; }

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
          std::cout << " MOVETPT: " << p << std::endl;
	  for (int i = 0; i < mapview->trk_layers.size(); ++i) {
            current_layer = dynamic_cast<LayerTRK *> (mapview->trk_layers[i].get());

            if (!mapview->workplane.get_layer_active(current_layer)) continue;
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_trackpoint(p);
	    if (d.first >= 0) {
		std::cout << "MOVETPT: found at " << current_layer << std::endl;
                g_map map = current_layer->get_ref();
                convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));

		current_tpt = &(current_layer->get_world()->trks[d.first][d.second]);
		if ((d.second > 0)&&(!current_layer->get_world()->trks[d.first][d.second].start)){
  		  dPoint p1 = current_layer->get_world()->trks[d.first][d.second-1];
		  cnv.bck(p1);
                  mapview->rubber.add_diag(p1);
		}
		if ((d.second < current_layer->get_world()->trks[d.first].size()-1)&&
		    (!current_layer->get_world()->trks[d.first][d.second+1].start)){
  		  dPoint p1 = current_layer->get_world()->trks[d.first][d.second+1];
		  cnv.bck(p1);
                  mapview->rubber.add_diag(p1);
		}
		if (mapview->rubber.size()==0)
                  mapview->rubber.add_diag(p);
                mystate=1;
		break;
            }
	  }
	} else { // move point
          assert (current_layer);
          g_map map = current_layer->get_ref();
          convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));
          dPoint pt(p.x, p.y);
          cnv.frw(pt);
          current_tpt->x = pt.x;
          current_tpt->y = pt.y;
          mapview->workplane.refresh_layer(current_layer);
	  mystate=0;
          mapview->rubber.clear();
          current_layer=0;
          current_tpt=0;
        }
    }

private:
    Mapview      * mapview;
    g_trackpoint * current_tpt;
    LayerTRK     * current_layer;
    int mystate; // 0 - select point, 1 - move point
};

#endif /* AM_MOVE_TPT_H */
