#ifndef DIALOGS_TRACE_H
#define DIALOGS_TRACE_H

#include <gtkmm.h>
#include "options/options.h"
#include "../widgets/rainbow.h"
#include "../widgets/comboboxes.h"

class DlgTrace : public Gtk::Dialog{

    Gtk::RadioButton  *dtr, *utr; // trace down/trace up
    Gtk::RadioButton  *pst, *lst; // point/line start
    Gtk::SpinButton   *ht;        // height threshold
    Gtk::SpinButton   *at;        // area threshold
    Gtk::SpinButton   *ma;        // max area
    Gtk::Adjustment   ht_adj, at_adj, ma_adj;

  public:
    DlgTrace();
    Options get_opt() const;
    void set_opt(const Options & o = Options());
};

#endif
