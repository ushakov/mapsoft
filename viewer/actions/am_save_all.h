#ifndef AM_SAVE_ALL
#define AM_SAVE_ALL

#include "action_mode.h"

class SaveAll : public ActionMode, public Gtk::FileSelection{
public:
    SaveAll (Mapview * mapview) :
           ActionMode(mapview), Gtk::FileSelection(get_name()){

      get_ok_button()->signal_clicked().connect(
          sigc::mem_fun (this, &SaveAll::on_ok));
      get_cancel_button()->signal_clicked().connect(
          sigc::mem_fun(this, &Gtk::Window::hide));
    }

    std::string get_name() { return "Save As/Export"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::SAVE_AS; }
//    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>s"); }
    bool is_radio() { return false; }

    void activate() { show(); }

    void on_ok(){
      std::string f = get_filename();
      mapview->statusbar.push("Save to " + f);
      try {io::out(f, mapview->get_world(false));}
      catch (MapsoftErr e) {mapview->dlg_err.call(e);}
      if (io::testext(f, ".xml")){
        mapview->set_filename(f);
        mapview->set_changed(false);
      }
      hide();
    }
};

#endif /* AM_SAVE_ALL */
