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
    Gtk::StockID get_stockid() { return Gtk::Stock::SAVE; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>s"); }
    bool is_radio() { return false; }

    void activate() { show(); }

    void on_ok(){
      std::string selected_filename = get_filename();
      g_print ("Saving to file: %s\n", selected_filename.c_str());

      geo_data world;
      if (mapview->data.size()<1) return;
      for (int i=0; i<mapview->data.size(); i++){
        world.wpts.insert( world.wpts.end(),
           mapview->data[i].get()->wpts.begin(),
           mapview->data[i].get()->wpts.end());
        world.trks.insert( world.trks.end(),
           mapview->data[i].get()->trks.begin(),
           mapview->data[i].get()->trks.end());
        world.maps.insert( world.maps.end(),
           mapview->data[i].get()->maps.begin(),
           mapview->data[i].get()->maps.end());
      }
      io::out(selected_filename, world, Options());
      hide();
    }
};

#endif /* AM_SAVE_ALL */
