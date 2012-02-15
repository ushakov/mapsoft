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
    Gtk::Entry *file;
    Gtk::CheckButton *map;
    Gtk::FileSelection file_d;

    Gtk::RadioButton  *rb_mpp_screen, *rb_mpp_auto, *rb_mpp_dpi;
    Gtk::SpinButton *mpp_dpi;
    Gtk::Adjustment dpi_adj;
    CBScale * mpp_scale;
    Gtk::Label *hint;

    PageBox *pagebox;
    void on_file_ch(int response);
    void on_ch(); 
    sigc::signal<void> signal_changed_;

  public:
    DlgSaveImg();
    std::string get_file() const;
    void set_file(const std::string & f);
    bool get_map() const;

    void set_px(const iPoint & p);
    iPoint get_px();

    double get_mpp();
    int get_mpp_style();

    void set_hint(const char * str);

    sigc::signal<void> & signal_changed(); // size or scale change

};

#endif
