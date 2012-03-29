#ifndef AM_DEL_TRK_H
#define AM_DEL_TRK_H

#include "action_mode.h"

class DelTrk : public ActionMode {
public:
    DelTrk (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Delete Track"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::DELETE; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = mapview->trk_ll.store->children().begin();
           i != mapview->trk_ll.store->children().end(); i++){
        if (!(*i)[mapview->trk_ll.columns.checked]) continue;

        boost::shared_ptr<LayerTRK> layer =
          (*i)[mapview->trk_ll.columns.layer];
        int d = layer->find_track(p, 3);
        if (d<0) continue;

        mapview->workplane.remove_layer(layer.get());
        mapview->trk_ll.store->erase(i);
        mapview->set_changed();
        mapview->refresh();
        break;
      }
    }
};

#endif /* AM_EDIT_TRACK_H */
