#ifndef AM_DOWNLOAD
#define AM_DOWNLOAD

#include "action_mode.h"
#include "../widgets.h"

class Download : public ActionMode {
public:
    Download (Mapview * mapview) : ActionMode(mapview) {

      dlg.signal_response().connect(
        sigc::mem_fun (this, &Download::on_response));

      dlg.e_dev->set_text("/dev/ttyUSB0");
      dlg.cb_w->set_active();
      dlg.cb_a->set_active();
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Download from GPS"; }

    bool is_radio() { return false; }

    void activate() { dlg.show_all(); }

    void on_response(int r){

      if (r!=Gtk::RESPONSE_OK) return;

      geo_data world;
      std::string dev = dlg.e_dev->get_text();

      if (dlg.cb_w->get_active()){
        if (!gps::get_waypoints (dev.c_str(), world, Options()))
          mapview->statusbar.push("Error while waypoint downloading",0);
      }

      if (dlg.cb_a->get_active() || dlg.cb_o->get_active()){
        if (!gps::get_tracks (dev.c_str(), world, Options()))
          mapview->statusbar.push("Error while track downloading",0);

        std::vector<g_track>::iterator i = world.trks.begin();
        while (i!=world.trks.end()){
          if ((!dlg.cb_a->get_active() && (i->comm=="ACTIVE LOG")) ||
              (!dlg.cb_o->get_active() && (i->comm!="ACTIVE LOG")))
            i=world.trks.erase(i);
          else i++;
        }
      }

      if (world.trks.size() || world.wpts.size())
        mapview->add_world(world);

      if (dlg.cb_off->get_active())
        gps::turn_off(dev.c_str());
    }

private:
    DlgDownload dlg;
};

#endif /* AM_DOWNLOAD */
