#ifndef AM_SAVE_ALL
#define AM_SAVE_ALL

#include "action_mode.h"

class SaveAll : public ActionMode, public Gtk::FileSelection{
public:
    SaveAll (Mapview * mapview) :
           ActionMode(mapview), Gtk::FileSelection("Save All"){

      get_ok_button()->signal_clicked().connect(
          sigc::mem_fun (this, &SaveAll::on_ok));
      get_cancel_button()->signal_clicked().connect(
          sigc::mem_fun(this, &Gtk::Window::hide));
    }

    std::string get_name() { return "Save All"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::SAVE_AS; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>s"); }
    bool is_radio() { return false; }

    void activate() { show(); }

    void on_ok(){
      std::string selected_filename = get_filename();
      g_print ("Saving to file: %s\n", selected_filename.c_str());

      io::out(selected_filename, mapview->get_world(false), Options());
      hide();
    }
};

#endif /* AM_SAVE_ALL */
