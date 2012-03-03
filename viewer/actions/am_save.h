#ifndef AM_SAVE
#define AM_SAVE

#include "action_mode.h"

class Save : public ActionMode, public Gtk::FileSelection{
public:
    Save (Mapview * mapview) :
           ActionMode(mapview), Gtk::FileSelection("Save"){

      get_ok_button()->signal_clicked().connect(
          sigc::mem_fun (this, &Save::on_ok));
      get_cancel_button()->signal_clicked().connect(
          sigc::mem_fun(this, &Gtk::Window::hide));
    }

    std::string get_name() { return "Save"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::SAVE; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>s"); }
    bool is_radio() { return false; }

    void activate() {
      std::string f = mapview->get_filename();
      set_filename(f); // set to dialog

      if (f != "") on_ok();
      else show();
    }

    void on_ok(){
      std::string f = get_filename();
      g_print ("Saving to file: %s\n", f.c_str());
      io::out(f, mapview->get_world(false), Options());
      if (io::testext(f, ".xml")){
        mapview->set_filename(f);
        mapview->set_changed(false);
      }
      hide();
    }
};

#endif /* AM_SAVE_ALL */
