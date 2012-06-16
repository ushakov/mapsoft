#ifndef AM_OPEN
#define AM_OPEN

#include "action_mode.h"

class Open : public ActionMode, public Gtk::FileSelection{
public:
    Open (Mapview * mapview) :
           ActionMode(mapview),
           Gtk::FileSelection(get_name()){

      Glib::RefPtr<Gtk::FileFilter> filter(new Gtk::FileFilter);
      filter->add_pattern("*.xml");
//      set_filter(filter);

      set_transient_for(*mapview);
      get_ok_button()->signal_clicked().connect(
          sigc::mem_fun (this, &Open::on_ok));
      get_cancel_button()->signal_clicked().connect(
          sigc::mem_fun(this, &Gtk::Window::hide));
    }

    std::string get_name() { return "Open mapsoft XML"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::OPEN; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>o"); }

    bool is_radio() { return false; }

    void activate() { show(); }

    void on_ok(){
      std::string f = get_filename();
      if (!io::testext(f, ".xml")){
        mapview->dlg_err.call(
          MapsoftErr() << "Only mapsoft xml files can be opened."
            " Use Add/Import to load other geodata formats.");
        return;
      }
      mapview->load_file(f);
      hide();
    }
};

#endif
