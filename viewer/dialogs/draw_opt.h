#ifndef DIALOGS_DRAW_OPT_H
#define DIALOGS_DRAW_OPT_H

#include <gtkmm.h>
#include "options/options.h"
#include "../widgets/rainbow.h"

class DlgDrawOpt : public Gtk::Dialog{

    Gtk::CheckButton  *dots, *arrows;
    Gtk::RadioButton  *m_normal, *m_speed, *m_height;
    Rainbow *rv, *rh;
    sigc::signal<void> signal_changed_;

    void on_ch(int mode);

  public:
    DlgDrawOpt();
    Options get_opt() const;
    void set_opt(const Options & o = Options());
    sigc::signal<void> & signal_changed();

};

#endif
