#ifndef DIALOGS_SHOW_PT_H
#define DIALOGS_SHOW_PT_H

#include <gtkmm.h>
#include "../widgets/coord_box.h"
#include "../widgets/nom_box.h"

// dialog for show point action
class DlgShowPt : public Gtk::Dialog{
    CoordBox *coord;
    NomBox   *nom;
    Gtk::Label *srtm_h;

    void jump(const dPoint p);
    sigc::signal<void, dPoint> signal_jump_;

  public:
    DlgShowPt();

    sigc::signal<void, dPoint> & signal_jump();
    void show_all(const dPoint & pt, double alt);
};

#endif
