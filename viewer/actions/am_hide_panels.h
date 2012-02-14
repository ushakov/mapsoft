#ifndef AM_HIDE_PANELS
#define AM_HIDE_PANELS

#include "action_mode.h"

class HidePanels : public ActionMode{
    int state;
public:

    HidePanels (Mapview * mapview) : ActionMode(mapview), state(0){ }

    std::string get_name() { return "Hide Panels"; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>h"); }
    bool is_radio() { return false; }

    void activate() {
      state = (state+1)%2;
      if (state==0){
        mapview->statusbar.show();
        mapview->dataview->show();
      }
      else if (state==1){
        mapview->statusbar.hide();
        mapview->dataview->hide();
      }
    }
};

#endif
