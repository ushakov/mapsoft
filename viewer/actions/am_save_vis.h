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

      geo_data world;

      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = mapview->ll_wpt.store->children().begin();
           i != mapview->ll_wpt.store->children().end(); i++){
         if (!(*i)[mapview->ll_wpt.columns.checked]) continue;
         geo_data * w = (*i)[mapview->ll_wpt.columns.world];
         assert(w);
         world.wpts.insert( world.wpts.end(), w->wpts.begin(), w->wpts.end());
      }
      for (i  = mapview->ll_trk.store->children().begin();
           i != mapview->ll_trk.store->children().end(); i++){
         if (!(*i)[mapview->ll_trk.columns.checked]) continue;
         geo_data * w = (*i)[mapview->ll_trk.columns.world];
         assert(w);
         world.trks.insert( world.trks.end(), w->trks.begin(), w->trks.end());
      }
      for (i  = mapview->ll_map.store->children().begin();
           i != mapview->ll_map.store->children().end(); i++){
         if (!(*i)[mapview->ll_map.columns.checked]) continue;
         geo_data * w = (*i)[mapview->ll_map.columns.world];
         assert(w);
         world.maps.insert( world.maps.end(), w->maps.begin(), w->maps.end());
      }
      io::out(selected_filename, world, Options());
      hide();
    }
};

#endif /* AM_SAVE_VIS */
