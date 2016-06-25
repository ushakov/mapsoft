#ifndef DIALOGS_SRTM_OPT_H
#define DIALOGS_SRTM_OPT_H

#include <gtkmm.h>
#include "options/options.h"
#include "../widgets/rainbow.h"
#include "../widgets/comboboxes.h"

class DlgSrtmOpt : public Gtk::Dialog{

    Gtk::CheckButton  *cnt;
    Gtk::RadioButton  *m_normal, *m_slopes;
    Gtk::SpinButton   *cnt_val;
    Gtk::Adjustment   cnt_val_adj;
    Gtk::Button       *dirbtn;
    Gtk::Label        *dir;
    Rainbow *rh, *rs;
    Gtk::FileSelection fdlg;

    sigc::signal<void> signal_changed_;
    void on_ch(int mode);

    void on_dirbtn();
    void on_fresult(int r);


  public:
    DlgSrtmOpt();
    Options get_opt() const;
    void set_opt(const Options & o = Options());
    sigc::signal<void> & signal_changed() { return signal_changed_; }

};

#endif
