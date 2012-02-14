#ifndef DIALOGS_TRK_FILTER_H
#define DIALOGS_TRK_FILTER_H

#include <gtkmm.h>
#include <geo/geo_data.h>

// dialog for Filter track action
class DlgTrkFilter : public Gtk::Dialog{
    Gtk::CheckButton *cb_acc, *cb_num, *cb_rg;
    Gtk::SpinButton *acc, *num;
    Gtk::Adjustment acc_adj, num_adj;
    Gtk::Label *info;
    void   toggle_acc();
    void   toggle_num();

  public:
    DlgTrkFilter();

    double get_acc() const;
    int    get_num() const;
    bool    get_rg() const;
    void set_info(const g_track * trk);

};

#endif
