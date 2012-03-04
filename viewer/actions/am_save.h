#ifndef AM_SAVE
#define AM_SAVE

#include "action_mode.h"

class Save : public ActionMode, public Gtk::FileSelection{
public:
    Save (Mapview * mapview) :

           ActionMode(mapview), Gtk::FileSelection("Save"),
           warn_dlg("Only mapview xml files can be saved. Save As/Export to"
                    " save other geodata formats. ", false,
                    Gtk::MESSAGE_WARNING, Gtk::BUTTONS_CLOSE){

      warn_dlg.signal_response().connect(
        sigc::hide(sigc::mem_fun(warn_dlg, &Gtk::MessageDialog::hide_all)));

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
      if (!mapview->get_changed()){
         mapview->statusbar.push("No changes to save");
         return;
      }
      std::string f = mapview->get_filename();
      set_filename(f); // set to dialog

      if (f != "") on_ok();
      else show();
    }

    void on_ok(){
      std::string f = get_filename();
      if (!io::testext(f, ".xml")){
         warn_dlg.show_all();
         return;
      }
      mapview->statusbar.push("Saving to " + f);
      io::out(f, mapview->get_world(false), Options());
      mapview->set_filename(f);
      hide();
    }
    Gtk::MessageDialog warn_dlg;
};

#endif /* AM_SAVE_ALL */
