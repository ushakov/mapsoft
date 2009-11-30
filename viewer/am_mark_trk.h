#ifndef AM_MARK_TRK_H
#define AM_MARK_TRK_H

#include <boost/lexical_cast.hpp>
#include <sstream>
#include <iomanip>

#include "action_mode.h"
#include "action_manager.h"
#include "mapview.h"

class MarkTrack : public ActionMode {
public:
     MarkTrack (Mapview * state_, Viewer * viewer_) : state(state_), viewer(viewer_) {
    }
    
    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Mark Track's length";
    }

    // Activates this mode.
    virtual void activate() {
    }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {
    }

    void add_wpt (g_waypoint_list * wpt_list, g_point where, double what) {
	std::ostringstream ss;
	g_waypoint wpt;
	ss << std::fixed << std::setprecision(1) << what/1000 << "km";
	wpt.name = ss.str();
	wpt.x = where.x;
	wpt.y = where.y;
	wpt_list->push_back(wpt);
    }
    
    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p) {
	for (int i = 0; i < state->trk_layers.size(); ++i) {
            LayerTRK * current_layer = dynamic_cast<LayerTRK *> (state->trk_layers[i].get());
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_trackpoint(p);
	    if (d.first >= 0) {
		g_track * track = & (current_layer->get_world()->trks[d.first]);
		int b, e, s;
		if (d.second * 2 > track->size()) {
		    b = track->size()-1;
		    e = -1;
		    s = -1;
		} else {
		    b = 0;
		    e = track->size();
		    s = 1;
		}
		if (current_layer->get_world()->wpts.size() == 0) {
		    current_layer->get_world()->wpts.push_back(g_waypoint_list());
		}
		g_waypoint_list * wpt_list = &(current_layer->get_world()->wpts[0]);
		add_wpt (wpt_list, (*track)[b], 0);

		convs::pt2pt pc(Datum("wgs84"), Proj("tmerc"), Options(),
                                Datum("wgs84"), Proj("lonlat"), Options());
		double len = 0;	
		g_trackpoint pp;
		double next = 1000;
		for (int i = b; i != e; i += s) {
		    g_trackpoint tp = (*track)[i];
		    pc.bck(tp); // координаты -- в tmerc
		    
		    if (i != b) {
			double this_len = hypot(tp.x - pp.x, tp.y - pp.y);
			while (len + this_len > next) {
			    g_point where;
			    double frac = (next - len) / this_len;
			    where = (*track)[i-1] * (1 - frac) + (*track)[i] * frac;
			    add_wpt (wpt_list, where, next);
			    next += 1000;
			}
			len += this_len;
		    }
		    pp = tp;
		}
		add_wpt(wpt_list, (*track)[e-s], len);
		viewer->workplane.refresh_layer(current_layer);
		return;
	    }
	}
    }

private:
    Mapview  * state;
    Viewer   * viewer;
};


#endif /* AM_MARK_TRK_H */
