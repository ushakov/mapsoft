#ifndef WIDGETS_PAGE_BOX_H
#define WIDGETS_PAGE_BOX_H

#include "comboboxes.h"
#include <2d/point.h>

// widget for page selection
class PageBox : public Gtk::Frame{
  CBPage *page;
  CBUnit *units, *m_units;

  Gtk::CheckButton *landsc;
  Gtk::SpinButton *marg, *x, *y, *dpi;
  Gtk::Adjustment marg_adj, dpi_adj, x_adj, y_adj;

  void ch_units(); // on units/dpi change
  void ch_page();  // on page/landscape/dpi change
  void ch_value(); // on x,y value change

  int old_mu, old_u;
  bool no_ch;
  sigc::signal<void> signal_changed_;

  public:
    PageBox();
    int get_dpi();
    dPoint get_px();
    void set_px(const dPoint & p);
    sigc::signal<void> & signal_changed();

};


#endif
