#ifndef AM_EDIT_TPT_H
#define AM_EDIT_TPT_H

#include "action_mode.h"
#include "../dialogs/trk_pt.h"

class EditTrackpoint : public ActionMode {
public:
    EditTrackpoint (Mapview * mapview) : ActionMode(mapview) {
      dlg.set_transient_for(*mapview);
      dlg.signal_jump().connect(
          sigc::mem_fun (this, &EditTrackpoint::on_jump));
      dlg.signal_response().connect(
        sigc::mem_fun (this, &EditTrackpoint::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Edit Trackpoint"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::EDIT; }

    void abort() {dlg.hide_all();}

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      int pt_num = mapview->find_tpt(p, &layer);
      if (pt_num < 0) return;
      tpt = layer->get_pt(pt_num);
      dlg.tpt2dlg(tpt);
      dlg.show_all();
    }

private:
    g_trackpoint * tpt;
    LayerTRK * layer;
    DlgTrkPt dlg;

    void on_result(int r) {
      if (r!=Gtk::RESPONSE_OK) return;
      dlg.dlg2tpt(tpt);
      mapview->set_changed();
      mapview->workplane.refresh_layer(layer);
    }
    void on_jump(dPoint p){
      mapview->cnv.bck(p);
      mapview->rubber.clear();
      mapview->viewer.set_center(p);
      mapview->rubber.add_src_mark(p);
    }
};

#endif /* AM_EDIT_TPT_H */
