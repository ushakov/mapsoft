#ifndef DIALOGS_SAVE_IMG_H
#define DIALOGS_SAVE_IMG_H

#include <gtkmm.h>
#include <2d/point.h>
#include "../widgets/page_box.h"

#define MPP_SCREEN 0
#define MPP_AUTO   1
#define MPP_MANUAL 2

// dialog for SaveImage action
class DlgSaveImg : public Gtk::Dialog{
    Gtk::CheckButton *map, *fig;
    Gtk::RadioButton  *rb_mpp_screen, *rb_mpp_auto, *rb_mpp_dpi;
    Gtk::SpinButton *mpp_dpi, *scr_mag;
    Gtk::Adjustment dpi_adj, mag_adj;
    CBScale  *mpp_scale;
    CBCorner *corner;
    Gtk::Label *hint;

    PageBox *pagebox;
    void on_ch(); 
    sigc::signal<void> signal_changed_;

  public:
    DlgSaveImg();
    bool get_map() const {return map->get_active();}
    bool get_fig() const {return fig->get_active();}

    void set_px(const iPoint & p);
    iPoint get_px();
    int get_dpi();

    double get_mpp() const;
    double get_scr_mag() const;
    int get_mpp_style() const;

    int get_corner() const;

    void set_hint(const char * str);

    sigc::signal<void> & signal_changed(); // size or scale change

};

#endif
