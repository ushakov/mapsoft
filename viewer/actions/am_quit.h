#ifndef AM_QUIT
#define AM_QUIT

#include "action_mode.h"

class Quit : public ActionMode{
public:
    Quit (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Quit"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::QUIT; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>q"); }
    bool is_radio() { return false; }
    void activate() { mapview->exit(); }
};

#endif /* AM_QUIT */
