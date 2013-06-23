#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <gtkmm.h>

class LayerListButtons : public Gtk::HBox {
public:
  LayerListButtons();
  Gtk::Button *up, *down, *save, *del, *jump;
};

class Mapview;

class DataView : public Gtk::Notebook {
    Mapview * mapview;
    void layer_del();
    void layer_jump();
    void layer_move(bool up);
    void layer_save();
    void on_gobj_save(int r);
    Gtk::FileSelection save_dlg;

  public:

    DataView (Mapview * M);
};


#endif /* DATAVIEW_H */
