#ifndef AM_REFMAP_H
#define AM_REFMAP_H

#include <sstream>
#include <boost/shared_ptr.hpp>

#include "action_mode.h"
#include "../mapview.h"

class RefMap : public ActionMode {
public:
    RefMap (Mapview * state_) : state(state_) { }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Reference a new map";
    }

    // Activates this mode.
    virtual void activate();

    // Abandons any action in progress and deactivates mode.
    virtual void abort();

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p);

private:
    Mapview       * state;

    bool working;
    
    LayerGeoMap * map_layer;
    boost::shared_ptr<LayerGeoMap> layer;

    boost::shared_ptr<geo_data> mymap;
    boost::shared_ptr<convs::map2pt> current_conv;

    double default_dpp; // default scale, degrees per image pixel
    dPoint default_origin; // default origin, lonlat
    iPoint size; // image size
    std::vector<g_refpoint> refpoints;
    bool last_refpoint_partial;

    int context;  // for statusbar

    void on_result(int r, const Options & o);
    void remake_ref();
};

#endif /* AM_REFMAP_H */
