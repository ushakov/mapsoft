#ifndef AM_SHOW_PTH
#define AM_SHOW_PTH

#include "action_mode.h"
#include <geo/geo_nom.h>
#include "../dialogs/show_pt.h"

class ShowPt : public ActionMode {
public:
    ShowPt (Mapview * mapview) : ActionMode(mapview) {

      dlg.signal_jump().connect(
          sigc::mem_fun (this, &ShowPt::on_jump));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Show point information"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::INFO; }

    void abort() {dlg.hide_all();}

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      if (!mapview->have_reference){
        mapview->statusbar.push("No geo-reference", 0);
        return;
      }
      mapview->rubber.clear();
      mapview->rubber.add_src_mark(p);

      convs::map2pt cnv(mapview->reference,
        Datum("wgs84"), Proj("lonlat"), Options());
      dPoint pt(p);
      cnv.frw(pt);
      dlg.show_all(pt);
    }

    void on_jump(dPoint p){
      convs::map2pt cnv(mapview->reference,
        Datum("wgs84"), Proj("lonlat"), Options());
      cnv.bck(p);
      mapview->rubber.clear();
      mapview->viewer.set_center(p);
      mapview->rubber.add_src_mark(p);
    }


private:
    DlgShowPt dlg;
};

#endif /* AM_SHOW_PTH */
