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
  public:

    DataView (Mapview * M);
};


#endif /* DATAVIEW_H */
