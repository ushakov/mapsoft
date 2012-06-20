#ifndef AM_DEL_TRK_H
#define AM_DEL_TRK_H

#include "action_mode.h"

class DelTrk : public ActionMode {
public:
    DelTrk (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Delete Track"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::DELETE; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      LayerTRK *layer;
      if (mapview->find_tpt(p, &layer, true)<0) return;
      mapview->workplane.remove_layer(layer);
      mapview->trk_ll.del_layer(layer);
      mapview->refresh();
    }
};

#endif
