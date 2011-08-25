#ifndef AM_MARK_TRK_H
#define AM_MARK_TRK_H

#include <boost/lexical_cast.hpp>
#include <sstream>
#include <iomanip>

#include "action_mode.h"
#include "2d/line_dist.h"

class MarkTrack : public ActionMode {
public:
     MarkTrack (Mapview * mapview) : ActionMode(mapview) {
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Mark Track's length";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() { }

    void add_wpt (g_waypoint_list & wpt_list, dPoint where, double what) {
	std::ostringstream ss;
	g_waypoint wpt;
	wpt.bgcolor=0xFF00FFFF;
	ss << std::fixed << std::setprecision(1) << what/1000 << "km";
	wpt.name = ss.str();
	wpt.x = where.x;
	wpt.y = where.y;
	wpt_list.push_back(wpt);
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) {
	for (int i = 0; i < mapview->trk_layers.size(); ++i) {
            LayerTRK * current_layer = dynamic_cast<LayerTRK *> (mapview->trk_layers[i].get());
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_trackpoint(p);

	    if (d.first < 0) continue;

	    g_track * track = & (current_layer->get_world()->trks[d.first]);

	    bool go_back = d.second * 2 > track->size();

	    int b;
	    if (go_back) b = track->size()-1;
	    else b = 0;

            Options o;
            o.put("lon0", convs::lon2lon0((*track)[b].x));
	    convs::pt2pt cnv(Datum("wgs84"), Proj("tmerc"), o,
                             Datum("wgs84"), Proj("lonlat"), Options());

	    g_waypoint_list wpt_list;

            dLine gk_track=(dLine)(*track);
            if (go_back) gk_track = gk_track.inv();
            cnv.line_bck_p2p(gk_track);

	    add_wpt (wpt_list, (*track)[b], 0);

    	    LineDist ld(gk_track);
    	    while (!ld.is_end()){
	      ld.move_frw(1000);
              dPoint p=ld.pt(); cnv.frw(p);
	      add_wpt (wpt_list, p, ld.dist());
	    }

	    // Add new layer for waypoints
	    boost::shared_ptr<geo_data> wpt_world (new geo_data);
    	    mapview->data.push_back(wpt_world);

	    boost::shared_ptr<LayerWPT> wpt_layer(new LayerWPT(wpt_world.get()));
	    wpt_layer->set_ref(mapview->reference);
	    mapview->wpt_layers.push_back(wpt_layer);
	    mapview->add_layer(wpt_layer.get(), 100, "wpt: track marks");

            wpt_world->wpts.push_back(wpt_list);

	    mapview->workplane.refresh_layer(wpt_layer.get());
	    return;

	}
    }

private:
    Mapview  * mapview;
};


#endif /* AM_MARK_TRK_H */
