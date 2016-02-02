#ifndef DIALOGS_TRK_MARK_H
#define DIALOGS_TRK_MARK_H

#include <gtkmm.h>

// dialog for Mark track action
class DlgMarkTrk : public Gtk::Dialog{
    Gtk::CheckButton *rev;
    Gtk::SpinButton *dist;
    Gtk::SpinButton *dist0;
    Gtk::Adjustment dist_adj, dist0_adj;

  public:
    DlgMarkTrk();

    bool    get_rev() const;
    void    set_rev(const bool r);
    double  get_dist() const;
    double  get_dist0() const;
};

#endif
