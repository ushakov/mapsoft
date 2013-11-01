#ifndef AM_SRTM_OPT_H
#define AM_SRTM_OPT_H

#include "action_mode.h"
#include "../dialogs/srtm_opt.h"

class SrtmOpt : public ActionMode {
public:
    SrtmOpt (Mapview * mapview) : ActionMode(mapview) {
      dlg.set_transient_for(*mapview);
      dlg.signal_response().connect(
        sigc::mem_fun (this, &SrtmOpt::on_response));
      dlg.signal_changed().connect(
        sigc::bind(sigc::mem_fun (this, &SrtmOpt::on_response),1));

      dlg.set_title(get_name());
    }

    std::string get_name() { return "SRTM options"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::PROPERTIES; }

    bool is_radio() { return false; }

    void activate() {
      o = mapview->panel_srtm.get_opt();
      dlg.set_opt(o);
      dlg.show_all();
    }

    void on_response(int r){
      if (r==Gtk::RESPONSE_CANCEL) mapview->panel_srtm.set_opt(o);
      if (r>0) mapview->panel_srtm.set_opt(dlg.get_opt());
      else dlg.hide_all();
    }

private:
    DlgSrtmOpt dlg;
    Options o;
};

#endif
