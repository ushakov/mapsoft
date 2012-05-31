#ifndef AM_SRTM_OPT_H
#define AM_SRTM_OPT_H

#include "action_mode.h"
#include "../dialogs/srtm_opt.h"

class SrtmOpt : public ActionMode {
public:
    SrtmOpt (Mapview * mapview) : ActionMode(mapview) {

      dlg.signal_response().connect(
        sigc::mem_fun (this, &SrtmOpt::on_response));
      dlg.signal_changed().connect(
        sigc::bind(sigc::mem_fun (this, &SrtmOpt::on_response),1));

      dlg.set_title(get_name());
      dlg.set_opt(mapview->layer_options);
    }

    std::string get_name() { return "SRTM layer"; }
//    Gtk::StockID get_stockid() { return Gtk::StockID(""); }

    bool is_radio() { return false; }

    void activate() {
      oldopts=mapview->layer_srtm.get_opt();
      oldopts.put<bool>("srtm_on",
        mapview->workplane.exists(&mapview->layer_srtm));
      dlg.set_opt(oldopts);
      dlg.show_all();
    }

    void on_response(int r){

      if (!mapview->have_reference){
        mapview->statusbar.push("No geo-reference", 0);
        return;
      }

      Options o = dlg.get_opt();
      if (r==Gtk::RESPONSE_OK) oldopts = o;
      if (r==Gtk::RESPONSE_CANCEL) o = oldopts;
      mapview->layer_srtm.set_opt(o);

      bool islayer = mapview->workplane.exists(&mapview->layer_srtm);
      bool isactive = o.get<bool>("srtm_on");

      if (islayer && !isactive){
        mapview->statusbar.push("SRTM OFF", 0);
        mapview->workplane.remove_layer(&mapview->layer_srtm);
        mapview->refresh();
      }
      else if (!islayer && isactive){
        mapview->statusbar.push("SRTM ON", 0);
        mapview->workplane.add_layer(&mapview->layer_srtm, 1000);
        mapview->layer_srtm.set_ref(mapview->reference);
        mapview->workplane.refresh_layer(&mapview->layer_srtm);
      }
      else if (islayer){
        mapview->workplane.refresh_layer(&mapview->layer_srtm);
      }

      if (r<0) dlg.hide_all();
    }

private:
    DlgSrtmOpt dlg;
    Options oldopts;
};

#endif
