#include "action_mode.h"
#include "../mapview.h"

int
ActionMode::find_wpt(const iPoint & p, LayerWPT ** layer,
                     int radius) const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = mapview->wpt_ll.store->children().begin();
       i != mapview->wpt_ll.store->children().end(); i++){
    if (!(*i)[mapview->wpt_ll.columns.checked]) continue;
    boost::shared_ptr<LayerWPT> current_layer=
      (*i)[mapview->wpt_ll.columns.layer];
    *layer = current_layer.get();
    int d = current_layer->find_waypoint(p, radius);
    if (d >= 0) return d;
  }
  *layer = NULL;
  return -1;
}

int
ActionMode::find_tpt(const iPoint & p, LayerTRK ** layer,
                     const bool segment, int radius) const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = mapview->trk_ll.store->children().begin();
       i != mapview->trk_ll.store->children().end(); i++){
    if (!(*i)[mapview->trk_ll.columns.checked]) continue;
    boost::shared_ptr<LayerTRK> current_layer=
      (*i)[mapview->trk_ll.columns.layer];
    *layer = current_layer.get();
    int d;
    if (segment) d = current_layer->find_track(p, radius);
    else d = current_layer->find_trackpoint(p, radius);
    if (d >= 0) return d;
  }
  *layer = NULL;
  return -1;
}

int
ActionMode::find_map(const iPoint & p, LayerGeoMap ** layer) const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = mapview->map_ll.store->children().begin();
       i != mapview->map_ll.store->children().end(); i++){
    if (!(*i)[mapview->map_ll.columns.checked]) continue;
    boost::shared_ptr<LayerGeoMap> current_layer=
      (*i)[mapview->map_ll.columns.layer];
    *layer = current_layer.get();
    int d = current_layer->find_map(p);
    if (d >= 0) return d;
  }
  *layer = NULL;
  return -1;
}


LayerWPT *
ActionMode::find_wpt_layer() const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = mapview->wpt_ll.store->children().begin();
       i != mapview->wpt_ll.store->children().end(); i++){
    if (!(*i)[mapview->wpt_ll.columns.checked]) continue;
    boost::shared_ptr<LayerWPT> current_layer=
      (*i)[mapview->wpt_ll.columns.layer];
    return current_layer.get();
  }
  return NULL;
}

LayerGeoMap *
ActionMode::find_map_layer() const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = mapview->map_ll.store->children().begin();
       i != mapview->map_ll.store->children().end(); i++){
    if (!(*i)[mapview->map_ll.columns.checked]) continue;
    boost::shared_ptr<LayerGeoMap> current_layer=
      (*i)[mapview->map_ll.columns.layer];
    return current_layer.get();
  }
  return NULL;
}
