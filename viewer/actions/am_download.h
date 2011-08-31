#ifndef AM_DOWNLOAD
#define AM_DOWNLOAD

#include "action_mode.h"
#include <geo/geo_nom.h>

class Download : public ActionMode {
public:
    Download (Mapview * mapview) : ActionMode(mapview) {
      builder = Gtk::Builder::create_from_file(
        "/usr/share/mapsoft/dialogs/download.xml");
      GETW("dlg", dlg)
      GETW("btn_ok", btn_ok)
      GETW("btn_cancel", btn_cancel)
      GETW("cb_w", cb_w)
      GETW("cb_a", cb_a)
      GETW("cb_o", cb_o)
      GETW("cb_off", cb_off)
      GETW("e_dev", e_dev)
      btn_ok->signal_clicked().connect(
          sigc::mem_fun (this, &Download::on_ok));
      btn_cancel->signal_clicked().connect(
          sigc::mem_fun(dlg, &Gtk::Window::hide));
      e_dev->set_text("/dev/ttyUSB0");
    }
    ~Download(){
      delete dlg, btn_ok, btn_cancel, cb_w, cb_a, cb_o;
    }

    std::string get_name() { return "Download from GPS"; }
    void activate() { dlg->show(); }

    void on_ok(){
      boost::shared_ptr<geo_data> world (new geo_data);

      std::string dev = e_dev->get_text();

      if (cb_w->get_active()){
        if (!gps::get_waypoints (dev.c_str(), *world, Options()))
          mapview->statusbar.push("Error while waypoint downloading",0);
      }

      if (cb_a->get_active() || cb_o->get_active()){
        if (!gps::get_tracks (dev.c_str(), *world, Options()))
          mapview->statusbar.push("Error while track downloading",0);

        std::vector<g_track>::iterator i = world->trks.begin();
        while (i!=world->trks.end()){
          if ((!cb_a->get_active() && (i->comm=="ACTIVE LOG")) ||
              (!cb_o->get_active() && (i->comm!="ACTIVE LOG")))
            i=world->trks.erase(i);
          else i++;
        }
      }

      if (world->trks.size() || world->wpts.size())
        mapview->add_world(world, "from GPS");

      if (cb_off->get_active())
        gps::turn_off(dev.c_str());

      dlg->hide();

    }

private:
    Glib::RefPtr<Gtk::Builder> builder;
    Gtk::Dialog *dlg;
    Gtk::Button *btn_ok, *btn_cancel;
    Gtk::CheckButton *cb_w, *cb_a, *cb_o, *cb_off;
    Gtk::Entry *e_dev;
};

#endif /* AM_DOWNLOAD */
