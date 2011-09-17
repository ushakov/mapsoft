#ifndef AM_DELETE_TPT_H
#define AM_DELETE_TPT_H

#include "action_mode.h"

class DeleteTrackpoint : public ActionMode {
public:
    DeleteTrackpoint (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Delete Trackpoint"; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      LayerTRK * layer;
      int d = find_tpt(p, &layer);
      if (d < 0) return;
      g_track * track = layer->get_data();
      bool start = layer->get_pt(d)->start;
      track->erase(track->begin()+d);
      if (start && (d < track->size())) (*track)[d].start = start;
      mapview->workplane.refresh_layer(layer);
    }
};

#endif /* AM_DELETE_TPT_H */
