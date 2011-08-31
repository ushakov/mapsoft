#ifndef AM_GOTO_LL_H
#define AM_GOTO_LL_H

#include <sstream>

#include "action_mode.h"
#include "../generic_dialog.h"

class GotoLL : public ActionMode {
public:
    GotoLL (Mapview * mapview) : ActionMode(mapview) { }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Goto LatLon";
    }

    // Activates this mode.
    virtual void activate() {
      Options opt;

      g_map map = mapview->reference;
      convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));

      dPoint P = mapview->viewer.get_origin();
      cnv.frw(P);
      opt.put("Longitude", P.x);
      opt.put("Latitude", P.y);
      mapview->gend.activate(get_name(), opt,
	    sigc::mem_fun(this, &GotoLL::on_result));
    }

private:

    void on_result(int r, const Options & o) {
       if (r == 0) { // OK
         g_map map = mapview->reference;
         convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));
         dPoint P(o.get("Longitude",0.0), o.get("Latitude",0.0));
         cnv.bck(P);
         mapview->viewer.set_origin(P);
       }
    }

};

#endif
