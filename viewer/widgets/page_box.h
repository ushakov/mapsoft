#ifndef WIDGETS_PAGE_BOX_H
#define WIDGETS_PAGE_BOX_H

#include "simple_combo.h"
#include <2d/point.h>

// widget for page selection
class PageBox : public Gtk::Frame{
    SimpleCombo<dPoint> * page;
    SimpleCombo<int> * units;

    Gtk::CheckButton *landsc;
    Gtk::SpinButton *marg, *x, *y, *dpi;
    Gtk::Adjustment marg_adj, x_adj, y_adj, dpi_adj;

  public:
    PageBox();
    dPoint get_px();
    void set_px(const dPoint & p);
};


#endif
