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
    boost::shared_ptr<LayerGeoMap> layer;

    geo_data mymap;

    void on_result(int r, const Options & o);
};

#endif /* AM_REFMAP_H */
