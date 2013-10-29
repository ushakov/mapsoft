#ifndef VIEWER_PANEL_COLS_H
#define VIEWER_PANEL_COLS_H

#include <gtkmm.h>

/* TreeModelColumnRecord for map/wpt/trk lists. */

template <typename Tl, typename Td>
class LayerTabCols : public Gtk::TreeModelColumnRecord {
public:
    Gtk::TreeModelColumn<bool> checked;
    Gtk::TreeModelColumn<std::string> comm;
    Gtk::TreeModelColumn<Pango::Weight> weight;
    Gtk::TreeModelColumn<boost::shared_ptr<Tl> > layer;
    Gtk::TreeModelColumn<boost::shared_ptr<Td> > data;

    LayerTabCols() {
      add(checked); add(comm); add(weight); add(layer); add(data);
    }
};

#endif
