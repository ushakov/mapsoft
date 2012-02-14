#ifndef DLG_PAGE_SEL_H
#define DLG_PAGE_SEL_H

/********************************************************************/
// dialog for page selection
class DlgPageSel : public Gtk::Table{
    SimpleCombo<dPoint> page;
    SimpleCombo<int> units;

    Gtk::CheckButton *landsc;
    Gtk::SpinButton *marg, *x, *y, *dpi;
    Gtk::Adjustment marg_adj, x_adj, y_adj, dpi_adj;

  public:
    dPoint get_px();
    void set_px(dPoint);
};


#endif
