#ifndef DIALOGS_SAVE_IMG_H
#define DIALOGS_SAVE_IMG_H

#include <gtkmm.h>
#include <2d/point.h>
#include "../widgets/comboboxes.h"

// dialog for SaveImage action
class DlgSaveImg : public Gtk::Dialog{
    Gtk::Entry *file;
    Gtk::Label *size;
    Gtk::CheckButton *map;
    Gtk::FileSelection file_d;

    Gtk::RadioButton  *rb_res_screen, *rb_res_dpi, *rb_res_mpp,
                      *rb_size_int, *rb_size_man, *rb_size_page;
    Gtk::Entry        *e_res_dpi, *e_res_scale, *e_res_mpp,
                      *e_size_x, *e_size_y;
    CBPage cb_page;
    Gtk::CheckButton *page_landsc;
    Gtk::SpinButton *page_marg;
    Gtk::Adjustment page_marg_adj;

  public:
    DlgSaveImg();
    std::string get_file() const;
    void set_file(const std::string & f);
    void on_file_ch(int response);
    void set_size(const int w, const int h);
    bool get_map() const;
};

#endif
