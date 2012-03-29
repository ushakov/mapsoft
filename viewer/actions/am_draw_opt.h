#ifndef AM_DRAW_OPT_H
#define AM_DRAW_OPT_H

#include "action_mode.h"
#include "../dialogs/draw_opt.h"

class DrawOpt : public ActionMode {
public:
    DrawOpt (Mapview * mapview) : ActionMode(mapview) {

      dlg.signal_response().connect(
        sigc::mem_fun (this, &DrawOpt::on_response));
      dlg.signal_changed().connect(
        sigc::bind(sigc::mem_fun (this, &DrawOpt::on_response),1));

      dlg.set_title(get_name());
      dlg.set_opt(mapview->layer_options);
    }

    std::string get_name() { return "Drawing options"; }
//    Gtk::StockID get_stockid() { return Gtk::StockID(""); }

    bool is_radio() { return false; }

    void activate() {
      dlg.show_all();
    }

    void on_response(int r){
      Options o = dlg.get_opt();
      if (r==Gtk::RESPONSE_OK) mapview->layer_options = o;
      if (r==Gtk::RESPONSE_CANCEL) o = mapview->layer_options;

      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = mapview->trk_ll.store->children().begin();
           i != mapview->trk_ll.store->children().end(); i++){
        boost::shared_ptr<LayerTRK> layer=
          (*i)[mapview->trk_ll.columns.layer];
        layer->set_opt(o);
        mapview->workplane.refresh_layer(layer.get());
      }
      if (r<0) dlg.hide_all();
    }

private:
    DlgDrawOpt dlg;
};

#endif
