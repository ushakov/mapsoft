#ifndef AM_GOTO_LL_H
#define AM_GOTO_LL_H

#include <sstream>

#include "action_mode.h"
#include "../generic_dialog.h"
#include "../mapview.h"

class GotoLL : public ActionMode {
public:
    GotoLL (Mapview * state_) : state(state_) { }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Goto LatLon";
    }

    // Activates this mode.
    virtual void activate() {
      Options opt;

      g_map map = state->reference;
      convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));

      g_point P = state->viewer.get_origin();
      cnv.frw(P);
      opt.put("Longitude", P.x);
      opt.put("Latitude", P.y);
      state->gend.activate(get_name(), opt,
	    sigc::mem_fun(this, &GotoLL::on_result));
    }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() { }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p) {}

private:
    Mapview       * state;

    void on_result(int r, const Options & o) {
       if (r == 0) { // OK
         g_map map = state->reference;
         convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));
         g_point P(o.get("Longitude",0.0), o.get("Latitude",0.0));
         cnv.bck(P);
         state->viewer.set_origin(P);
       }
    }

};

#endif
