#ifndef AM_SAVE_VIS
#define AM_SAVE_VIS

#include "action_mode.h"

class SaveVis : public ActionMode, public Gtk::FileSelection{
public:
    SaveVis (Mapview * mapview) :
           ActionMode(mapview), Gtk::FileSelection("Save Visible"){

      get_ok_button()->signal_clicked().connect(
          sigc::mem_fun (this, &SaveVis::on_ok));
      get_cancel_button()->signal_clicked().connect(
          sigc::mem_fun(this, &Gtk::Window::hide));
    }

    std::string get_name() { return "Save Visible"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::SAVE; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<shift><control>s"); }
    bool is_radio() { return false; }

    void activate() { show(); }

    void on_ok(){
      std::string selected_filename = get_filename();
      g_print ("Saving to file: %s\n", selected_filename.c_str());

      io::out(selected_filename, mapview->get_world(true), Options());
      hide();
    }
};

#endif /* AM_SAVE_VIS */
