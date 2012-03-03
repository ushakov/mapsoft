#ifndef AM_NEW
#define AM_NEW

#include "action_mode.h"

class New : public ActionMode{
public:
    New (Mapview * mapview) : ActionMode(mapview){ }

    std::string get_name() { return "New"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::NEW; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>n"); }
    bool is_radio() { return false; }

    void activate() {
      mapview->new_file();
    }
};

#endif
