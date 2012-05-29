#ifndef AM_SRTM
#define AM_SRTM

#include "action_mode.h"

class UseSrtm : public ActionMode{
public:
    UseSrtm (Mapview * mapview) : ActionMode(mapview){ }

    std::string get_name() { return "Switch SRTM layer"; }
//    Gtk::StockID get_stockid() { return Gtk::Stock::ADD; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>h"); }

    bool is_radio() { return false; }

    void activate() {
      if (mapview->workplane.exists(&mapview->layer_srtm)){
        std::cerr << "SRTM off\n";
        mapview->workplane.remove_layer(&mapview->layer_srtm);
        mapview->refresh();
      }
      else{
        std::cerr << "SRTM on\n";
        mapview->workplane.add_layer(&mapview->layer_srtm, 1000);
        mapview->layer_srtm.set_ref(mapview->reference);
        mapview->workplane.refresh_layer(&mapview->layer_srtm);
      }
    }
};

#endif /* AM_ADD_FILE */
