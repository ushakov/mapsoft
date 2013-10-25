#ifndef DIALOGS_WPT_H
#define DIALOGS_WPT_H

#include <gtkmm.h>
#include <geo/geo_data.h>
#include "../widgets/coord_box.h"

// dialog for Add/Edit Waypoint actions
class DlgWpt : public Gtk::Dialog{
    CoordBox * coord;
    Gtk::ColorButton *fg, *bg;
    Gtk::Entry *name, *comm, *alt, *time;
    Gtk::SpinButton *fs, *ps;
    Gtk::Adjustment fs_adj, ps_adj;

  public:
    DlgWpt();

    void dlg2wpt(g_waypoint * wpt) const;
    void wpt2dlg(const g_waypoint * wpt);

    sigc::signal<void, dPoint> signal_jump();

    void set_ll(dPoint p);
};

#endif
