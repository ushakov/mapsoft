#ifndef DIALOGS_DRAW_OPT_H
#define DIALOGS_DRAW_OPT_H

#include <gtkmm.h>
#include "options/options.h"

class DlgDrawOpt : public Gtk::Dialog{

    Gtk::CheckButton  *dots, *arrows;
    Gtk::RadioButton  *m_normal, *m_speed, *m_height;
    Gtk::SpinButton *v1, *v2, *h1, *h2;

    Gtk::Adjustment v1a, v2a, h1a, h2a;
    sigc::signal<void> signal_change_;

  public:
    DlgDrawOpt();
    Options get_opt() const;
    void set_opt(const Options & o = Options());
    void on_ch();
    sigc::signal<void> & signal_change();

};

#endif
