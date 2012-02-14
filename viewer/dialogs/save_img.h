#ifndef DIALOGS_SAVE_IMG_H
#define DIALOGS_SAVE_IMG_H

#include <gtkmm.h>
#include <2d/point.h>
#include "../widgets/page_box.h"

// dialog for SaveImage action
class DlgSaveImg : public Gtk::Dialog{
    Gtk::Entry *file;
    Gtk::CheckButton *map;
    Gtk::FileSelection file_d;

    Gtk::RadioButton  *rb_res_screen, *rb_res_dpi, *rb_res_mpp;
    Gtk::Entry        *e_res_dpi, *e_res_scale, *e_res_mpp;

    PageBox *pagebox;

  public:
    DlgSaveImg();
    std::string get_file() const;
    void set_file(const std::string & f);
    void on_file_ch(int response);
    void set_px(const iPoint & p);
    iPoint get_px();
    bool get_map() const;

    sigc::signal<void> & signal_changed();

};

#endif
