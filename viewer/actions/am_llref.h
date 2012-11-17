#ifndef AM_LLREF
#define AM_LLREF

#include "action_mode.h"

class LLRef : public ActionMode{
public:
    LLRef (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Set LonLat ref"; }
    bool is_radio() { return false; }
    void activate() {
      g_map ref;
      int w = mapview->viewer.get_width();
      int h = w/2;

      ref.map_proj=Proj("lonlat");
      ref.push_back(g_refpoint(-180, 90, 0,0));
      ref.push_back(g_refpoint( 180, 90, w,0));
      ref.push_back(g_refpoint( 180,-90, w,h));
      ref.push_back(g_refpoint(-180,-90, 0,h));
      mapview->set_ref(ref);
      mapview->viewer.set_center(iPoint(w/2,h/2));
    }
};

#endif
