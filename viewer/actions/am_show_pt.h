#ifndef AM_SHOW_PT_H
#define AM_SHOW_PT_H

#include "action_mode.h"
#include <geo/geo_nom.h>
#include "../dialogs/show_pt.h"

class ShowPt : public ActionMode {
public:
    ShowPt (Mapview * mapview) : ActionMode(mapview) {
      dlg.set_transient_for(*mapview);
      dlg.signal_jump().connect(
          sigc::mem_fun (this, &ShowPt::on_jump));
      dlg.signal_response().connect(
        sigc::hide(sigc::mem_fun (this, &ShowPt::abort)));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Show point information"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::INFO; }

    void abort() {
      dlg.hide_all();
      mapview->rubber.clear();
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      if (!mapview->have_reference){
        mapview->statusbar.push("No geo-reference", 0);
        return;
      }
      mapview->rubber.clear();
      mapview->rubber.add_src_mark(p);

      dPoint pt(p);
      mapview->get_cnv()->frw(pt);
      dlg.show_all(pt, mapview->panel_srtm.srtm.geth4(pt));
    }

    void on_jump(dPoint p){
      mapview->get_cnv()->bck(p);
      mapview->rubber.clear();
      mapview->viewer.set_center(p);
      mapview->rubber.add_src_mark(p);
    }


private:
    DlgShowPt dlg;
};

#endif /* AM_SHOW_PTH */
