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
  Gtk::Button       *state_btn;

  Gtk::Entry *time, *alt;
  CoordBox          *crd;

  GPS_OPvt_Data * pvt;
  gpsdevh * gh;

  void on_update(); // connected to signal_update:
                    // update coords, emit signal_changed

  void updater();         // updater thread
  void restart_updater();
  void stop_updater();
  void auto_dev();        // device autodetection
  void set_state(int v);  // set state icon

  Glib::Thread     *thread;
  Glib::Dispatcher  signal_update; // emited from thread when new data is available
  bool updater_needed;
  int  updater_res; // result of updater thread: -1: err, 0: stopped, 1: ok

  sigc::signal<void, dPoint> signal_changed_;
  sigc::signal<void, dPoint> signal_goto_;
};

#endif
