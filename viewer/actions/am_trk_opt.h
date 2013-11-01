#ifndef AM_TRACK_OPT_H
#define AM_TRACK_OPT_H

#include "action_mode.h"
#include "../dialogs/draw_opt.h"

class DrawOpt : public ActionMode {
public:
    DrawOpt (Mapview * mapview) : ActionMode(mapview) {
      dlg.set_transient_for(*mapview);
      dlg.signal_response().connect(
        sigc::mem_fun (this, &DrawOpt::on_response));
      dlg.signal_changed().connect(
        sigc::bind(sigc::mem_fun (this, &DrawOpt::on_response),1));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Track drawing options"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::PROPERTIES; }

    bool is_radio() { return false; }

    void activate() {
      o = mapview->panel_trks.get_opt();
      dlg.set_opt(o);
      dlg.show_all();
    }

    void on_response(int r){
      if (r==Gtk::RESPONSE_OK)
        mapview->panel_trks.set_opt(dlg.get_opt());
      if (r==Gtk::RESPONSE_CANCEL)
        mapview->panel_trks.set_opt(o);
      if (r<0) dlg.hide_all();
    }

private:
    DlgDrawOpt dlg;
    Options o;
};

#endif
