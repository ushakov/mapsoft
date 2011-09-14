#ifndef AM_ADD_FILE
#define AM_ADD_FILE

#include "action_mode.h"

class AddFile : public ActionMode, public Gtk::FileSelection{
public:
    AddFile (Mapview * mapview) :
           ActionMode(mapview), Gtk::FileSelection("Add file"){

      get_ok_button()->signal_clicked().connect(
          sigc::mem_fun (this, &AddFile::on_ok));
      get_cancel_button()->signal_clicked().connect(
          sigc::mem_fun(this, &Gtk::Window::hide));
    }

    std::string get_name() { return "Add from file"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::ADD; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>a"); }
    bool is_radio() { return false; }

    void activate() { show(); }

    void on_ok(){
      mapview->add_file(get_filename());
      hide();
    }
};

#endif /* AM_ADD_FILE */
