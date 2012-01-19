#ifndef AM_ADD_FILE
#define AM_ADD_FILE

#include "action_mode.h"

class AddFile : public ActionMode, public Gtk::FileSelection{
public:
    AddFile (Mapview * mapview) :
           ActionMode(mapview), Gtk::FileSelection("Add file"){

      set_select_multiple();
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
      std::list<std::string> l = get_selections();
      geo_data world;
      for (std::list<std::string>::const_iterator i=l.begin(); i!=l.end(); i++)
        io::in(*i, world, Options());
      mapview->add_world(world, true);
      hide();
    }
};

#endif /* AM_ADD_FILE */
