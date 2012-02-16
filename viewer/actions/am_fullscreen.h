#ifndef AM_FULLSCREEN
#define AM_FULLSCREEN

#include "action_mode.h"

class FullScreen : public ActionMode{
    int state;
public:

    FullScreen (Mapview * mapview) : ActionMode(mapview), state(0){ }

    std::string get_name() { return "Fullscreen"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::FULLSCREEN; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>f"); }
    bool is_radio() { return false; }

    void activate() {
      state = (state+1)%2;
      if (state==0){
        mapview->unfullscreen();
      }
      else if (state==1){
        mapview->fullscreen();
      }
    }
};

#endif
