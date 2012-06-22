#ifndef DIALOGS_NAV_H
#define DIALOGS_NAV_H

/// Real-time navigation dialog and updater

#include <gtkmm.h>
#include "jeeps/gps.h"
#include "options/options.h"
#include "gred/viewer.h"
#include "../widgets/coord_box.h"

class DlgNav : public Gtk::Dialog{
  GPS_OPvt_Data data;

public:
  DlgNav();
  ~DlgNav();

  sigc::signal<void, dPoint> signal_changed(){return signal_changed_;}
  sigc::signal<void, dPoint> & signal_goto(){return signal_goto_;}

private:
  Gtk::CheckButton  *sw_upd, *sw_goto, *sw_trk;
  Gtk::Entry        *device;
  CoordBox          *crd;
  Gtk::Image        *state_icon;

  GPS_OPvt_Data * pvt;
  gpsdevh * gh;

  void on_update();    // connected to signal_update:
                       //   update coords, emit signal_changed
  void on_sw_update(); // connected to sw_upd: start/stop updater thread
  void updater();

  Options opt;
  Glib::Thread     *thread;
  Glib::Mutex      mutex;
  Glib::Dispatcher  signal_update; // emited from thread when new data is available
  bool updater_needed;

  sigc::signal<void, dPoint> signal_changed_;
  sigc::signal<void, dPoint> signal_goto_;
};

#endif
