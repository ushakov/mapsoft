#ifndef AM_PANO_H
#define AM_PANO_H

#include "action_mode.h"
#include "../dialogs/pano.h"

class Pano : public ActionMode {
public:
    Pano (Mapview * mapview) :
      ActionMode(mapview),
      dlg(&mapview->panel_srtm.srtm),
      state(0)
    {
      dlg.signal_response().connect(
        sigc::hide(sigc::mem_fun (this, &Pano::abort)));
      dlg.signal_go().connect(
        sigc::mem_fun (this, &Pano::on_go));
      dlg.signal_point().connect(
        sigc::mem_fun (this, &Pano::on_point));
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
        mapview->spanel.message("No geo-reference");
        return;
      }

      if (state==0 || mod&Gdk::CONTROL_MASK){ // first click
        p0=p; mapview->get_cnv()->frw(p0); state=1;
        mapview->rubber.clear();
        mapview->rubber.add_src_mark(p);
        dlg.show_all();
        dlg.set_origin(p0);
      }
      else{ // next click
        dPoint p0i(p0), p1(p);
        mapview->get_cnv()->bck(p0i); mapview->get_cnv()->frw(p1);
        mapview->rubber.clear();
        mapview->rubber.add_src_mark(p0i);
        mapview->rubber.add_src_mark(p);
        mapview->rubber.add_line(p,p0i);
        dlg.set_dir(p1);
      }
    }

    void on_point(dPoint p){
      dPoint p0i(p0);
      mapview->get_cnv()->bck(p); mapview->get_cnv()->bck(p0i);
      mapview->rubber.clear();
      mapview->rubber.add_src_mark(p0i);
      mapview->rubber.add_src_mark(p);
      mapview->rubber.add_line(p,p0i);
    }

    void on_go(dPoint p){
      dlg.set_origin(p);
      p0=p; mapview->get_cnv()->bck(p); state=1;
      mapview->rubber.clear();
      mapview->rubber.add_src_mark(p);
      mapview->viewer.set_center(p);
    }

private:
    DlgPano dlg;
    int state; // first/next click;
    dPoint p0;
};

#endif /* AM_SHOW_PTH */
