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
      for (i  = mapview->wpt_ll.store->children().begin();
           i != mapview->wpt_ll.store->children().end(); i++){
         if (!(*i)[mapview->wpt_ll.columns.checked]) continue;
         boost::shared_ptr<g_waypoint_list> w =
           (*i)[mapview->wpt_ll.columns.data];
         w->comm=(*i)[mapview->wpt_ll.columns.comm];
         world.wpts.push_back(*w);
      }
      for (i  = mapview->trk_ll.store->children().begin();
           i != mapview->trk_ll.store->children().end(); i++){
         if (!(*i)[mapview->trk_ll.columns.checked]) continue;
         boost::shared_ptr<g_track> w =
           (*i)[mapview->trk_ll.columns.data];
         w->comm=(*i)[mapview->trk_ll.columns.comm];
         world.trks.push_back(*w);
      }
      for (i  = mapview->map_ll.store->children().begin();
           i != mapview->map_ll.store->children().end(); i++){
         if (!(*i)[mapview->map_ll.columns.checked]) continue;
         boost::shared_ptr<g_map_list> w =
           (*i)[mapview->map_ll.columns.data];
         w->comm=(*i)[mapview->map_ll.columns.comm];
         world.maps.push_back(*w);
      }
      io::out(selected_filename, world, Options());
      hide();
    }
};

#endif /* AM_SAVE_VIS */
