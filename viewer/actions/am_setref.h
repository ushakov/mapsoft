#ifndef AM_SETREF_H
#define AM_SETREF_H

#include "action_mode.h"

class SetRef : public ActionMode {
public:
    SetRef (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Set Ref from map"; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      // find map
      LayerMAP * layer;
      int map_num = mapview->find_map(p, &layer);
      if (map_num < 0) return;

      dPoint pw(p); mapview->cnv.frw(pw);
      mapview->set_ref(*(layer->get_map(map_num)));
      mapview->goto_wgs(pw);
    }
};

#endif