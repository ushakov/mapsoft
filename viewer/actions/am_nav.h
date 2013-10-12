#ifndef AM_NAV_H
#define AM_NAV_H

#include "action_mode.h"
#include "../dialogs/nav.h"
#include "gred/rubber.h"

class Nav : public ActionMode {
public:
    Nav (Mapview * mapview) : ActionMode(mapview), R(&mapview->viewer) {
      dlg.set_transient_for(*mapview);

      dlg.signal_changed().connect(
        sigc::mem_fun (this, &Nav::on_change));
      dlg.signal_goto().connect(
        sigc::mem_fun (this, &Nav::on_goto));

      dlg.set_title(get_name());
    }

    std::string get_name() { return "Navigation"; }
//    Gtk::StockID get_stockid() { return Gtk::StockID(""); }

    bool is_radio() { return false; }

    void activate() {
      dlg.show_all();
    }

    void on_change(dPoint p){
      R.clear();
      mapview->get_cnv()->bck(p);
      R.add_src_mark(p);
    }

    void on_goto(dPoint p){
      mapview->get_cnv()->bck(p);
      mapview->viewer.set_center(p);
    }

private:
    DlgNav dlg;
    Rubber R;

    g_track trk;
};

#endif
