#ifndef AM_PANO_H
#define AM_PANO_H

#include "action_mode.h"
#include "../dialogs/pano.h"

class Pano : public ActionMode {
public:
    Pano (Mapview * mapview) :
      ActionMode(mapview),
      dlg(&mapview->srtm),
      state(0)
    {
      dlg.signal_response().connect(
        sigc::hide(sigc::mem_fun (this, &Pano::abort)));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Panoramic view"; }
//    Gtk::StockID get_stockid() { return Gtk::Stock::INFO; }

    void abort() {
      state=0;
      dlg.hide_all();
      mapview->rubber.clear();
    }

    void handle_click(iPoint p, const Gdk::ModifierType & mod) {
      if (!mapview->have_reference){
        mapview->statusbar.push("No geo-reference", 0);
        return;
      }

      convs::map2pt cnv(mapview->reference,
        Datum("wgs84"), Proj("lonlat"), Options());

      if (state==0){ // first click
        p0=p; state=1;
        mapview->rubber.clear();
        mapview->rubber.add_src_mark(p);
//        mapview->rubber.add_line(p);
        dPoint pt0(p);
        cnv.frw(pt0);
        dlg.show_all(pt0);
      }
      else{ // next click
        mapview->rubber.clear();
        mapview->rubber.add_src_mark(p0);
        mapview->rubber.add_src_mark(p);
        mapview->rubber.add_line(p,p0);
        dPoint pt1(p);
        cnv.frw(pt1);
        dlg.set_dir(pt1);
      }

    }

private:
    DlgPano dlg;
    int state; // first/next click;
    iPoint p0;
};

#endif /* AM_SHOW_PTH */
