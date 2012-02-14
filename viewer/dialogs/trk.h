#ifndef DIALOGS_TRK_H
#define DIALOGS_TRK_H

#include <gtkmm.h>
#include <geo/geo_data.h>

// dialog for Add/Edit Track actions
class DlgTrk : public Gtk::Dialog{
    Gtk::ColorButton *fg;
    Gtk::Entry *comm;
    Gtk::SpinButton *width;
    Gtk::Label *info;
    Gtk::Adjustment width_adj;
    Gtk::Label *hint;

  public:
    DlgTrk();

    void dlg2trk(g_track * trk) const;
    void trk2dlg(const g_track * trk);
    void set_info(const g_track * trk);
    void set_hint(const char * str);
};

#endif
