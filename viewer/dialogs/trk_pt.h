#ifndef DIALOGS_TRK_PT_H
#define DIALOGS_TRK_PT_H

#include <gtkmm.h>
#include "../widgets/coord_box.h"
#include <geo/geo_data.h>

// dialog for Edit trackpoint action
class DlgTrkPt : public Gtk::Dialog{
    CoordBox * coord;
    Gtk::Entry *time, *alt;
    Gtk::CheckButton *start;

  public:
    DlgTrkPt();

    void dlg2tpt(g_trackpoint * tpt) const;
    void tpt2dlg(const g_trackpoint * tpt);

    sigc::signal<void, dPoint> signal_jump();
};

#endif
