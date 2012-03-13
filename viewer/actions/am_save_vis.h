#ifndef AM_SAVE_VIS
#define AM_SAVE_VIS

#include "action_mode.h"

class SaveVis : public ActionMode, public Gtk::FileSelection{
public:
    SaveVis (Mapview * mapview) :
           ActionMode(mapview), Gtk::FileSelection(get_name()){

      get_ok_button()->signal_clicked().connect(
          sigc::mem_fun (this, &SaveVis::on_ok));
      get_cancel_button()->signal_clicked().connect(
          sigc::mem_fun(this, &Gtk::Window::hide));
    }

    std::string get_name() { return "Save Visible As"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::SAVE_AS; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<shift><control>s"); }
    bool is_radio() { return false; }

    void activate() { show(); }

    void on_ok(){
      std::string f = get_filename();
      mapview->statusbar.push("Save visible data to " + f);
      try {io::out(f, mapview->get_world(true));}
      catch (MapsoftErr e) {mapview->dlg_err.call(e);}
      hide();
    }
};

#endif /* AM_SAVE_VIS */
