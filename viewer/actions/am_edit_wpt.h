#ifndef AM_EDIT_WPT_H
#define AM_EDIT_WPT_H

#include "action_mode.h"
#include "../dialogs/wpt.h"

class EditWaypoint : public ActionMode {
public:
    EditWaypoint (Mapview * mapview) : ActionMode(mapview) { 
      dlg.signal_jump().connect(
          sigc::mem_fun (this, &EditWaypoint::on_jump));
      dlg.signal_response().connect(
        sigc::mem_fun (this, &EditWaypoint::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Edit Waypoint"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::EDIT; }

    void abort() { dlg.hide_all(); }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      pt_num=mapview->find_wpt(p, &layer);
      if (pt_num < 0) return;

      g_waypoint * wpt = layer->get_pt(pt_num);
      dlg.wpt2dlg(wpt);
      dlg.show_all();
    }

private:
    DlgWpt dlg;
    int pt_num;
    LayerWPT * layer;

    void on_result(int r) {
      mapview->rubber.clear();
      if ((pt_num<0) || (r!=Gtk::RESPONSE_OK)) return;
      dlg.dlg2wpt(layer->get_pt(pt_num));
      mapview->workplane.refresh_layer(layer);
    }

    void on_jump(dPoint p){
      convs::map2pt cnv(mapview->reference,
        Datum("wgs84"), Proj("lonlat"), Options());
      cnv.bck(p);
      mapview->rubber.clear();
      mapview->viewer.set_center(p);
      mapview->rubber.add_src_mark(p);
    }
};

#endif /* AM_EDIT_WPT_H */
