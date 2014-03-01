#ifndef AM_HIDE_PANELS
#define AM_HIDE_PANELS

#include "action_mode.h"

class HidePanels : public ActionMode{
    int state;
public:

    HidePanels (Mapview * mapview) : ActionMode(mapview), state(0){ }

    std::string get_name() { return "Hide/Show Panels"; }
    Gtk::AccelKey get_acckey() { return Gtk::AccelKey("<control>h"); }
    bool is_radio() { return false; }

    void activate() {
      state = (state+1)%2;
      if (state==0){
        mapview->spanel.show();
        mapview->panels->show();
      }
      else if (state==1){
        mapview->spanel.hide();
        mapview->panels->hide();
      }
    }
};

#endif
