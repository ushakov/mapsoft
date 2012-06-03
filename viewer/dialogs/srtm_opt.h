#ifndef DIALOGS_SRTM_OPT_H
#define DIALOGS_SRTM_OPT_H

#include <gtkmm.h>
#include "options/options.h"
#include "../widgets/rainbow.h"

class DlgSrtmOpt : public Gtk::Dialog{

    Gtk::CheckButton  *cnt;
    Gtk::RadioButton  *m_normal, *m_slopes;
    Gtk::SpinButton   *cnt_val;
    Gtk::Adjustment   cnt_val_adj;
    Rainbow *rh, *rs;

    sigc::signal<void> signal_changed_;
    void on_ch(int mode);

  public:
    DlgSrtmOpt();
    Options get_opt() const;
    void set_opt(const Options & o = Options());
    sigc::signal<void> & signal_changed();

};

#endif
