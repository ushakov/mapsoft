#ifndef AM_ADD_FILE
#define AM_ADD_FILE

#include "action_mode.h"

class AddFile : public ActionMode, public Gtk::FileSelection{
public:
    AddFile (Mapview * mapview) :
           ActionMode(mapview), Gtk::FileSelection("Add/Import"){

      Glib::RefPtr<Gtk::FileFilter> filter(new Gtk::FileFilter);
      filter->add_pattern("*.xml");
      filter->add_pattern("*.gu");
      filter->add_pattern("*.gpx");
      filter->add_pattern("*.plt");
      filter->add_pattern("*.wpt");
      filter->add_pattern("*.map");
      filter->add_pattern("*.fig");
//      set_filter(filter);

      set_select_multiple();
      get_ok_button()->signal_clicked().connect(
          sigc::mem_fun (this, &AddFile::on_ok));
      get_cancel_button()->signal_clicked().connect(
          sigc::mem_fun(this, &Gtk::Window::hide));
    }

    std::string get_name() { return "Add/Import"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::ADD; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>a"); }
    bool is_radio() { return false; }

    void activate() { show(); }

    void on_ok(){
      mapview->add_files(get_selections());
      hide();
    }
};

#endif /* AM_ADD_FILE */
