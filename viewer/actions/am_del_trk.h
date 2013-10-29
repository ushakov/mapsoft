#ifndef AM_DEL_TRK_H
#define AM_DEL_TRK_H

#include "action_mode.h"

class DelTrk : public ActionMode {
public:
    DelTrk (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Delete Track"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::DELETE; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      GObjTRK *gobj;
      if (mapview->panel_trks.find_tpt(p, &gobj, true)<0) return;
      mapview->panel_trks.remove_gobj(gobj);
      mapview->refresh();
    }
};

#endif
