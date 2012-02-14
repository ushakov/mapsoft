#ifndef WIDGETS_COORD_BOX_H
#define WIDGETS_COORD_BOX_H

#include "simple_combo.h"
#include <geo/geo_convs.h>

/*
  Geo-coordinates selection widget.
  See "show point information" or "add waypoint" dialogs
  in mapview for example.
*/

class CoordBox : public Gtk::Frame {
public:
  CoordBox();
  CoordBox(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

  void set_ll(const dPoint &p);
  dPoint get_ll();
  sigc::signal<void> & signal_changed();
  sigc::signal<void, dPoint> & signal_jump();

private:
  Gtk::Entry coords;
  Datum datum;
  Proj proj;
  SimpleCombo<Proj>  proj_cb;
  SimpleCombo<Datum> datum_cb;
  sigc::signal<void> signal_changed_;
  sigc::signal<void, dPoint> signal_jump_;
  dPoint old_pt; // to fix incorrect values

  dPoint get_xy();
  void init();
  void on_conv();
  void on_change();
  void on_jump();
};

#endif
