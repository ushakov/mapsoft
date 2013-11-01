#include "trks_panel.h"
#include "../mapview.h"
#include "../mapview.h"

void
PanelTRK::add(const boost::shared_ptr<g_track> data) {
  boost::shared_ptr<GObjTRK>
    layer(new GObjTRK(data.get(), mapview->panel_options));
  add_gobj(layer.get(), 0);
  if (!mapview->have_reference)
    mapview->set_ref(layer->get_myref());
  Gtk::TreeModel::iterator it = store->append();
  Gtk::TreeModel::Row row = *it;
  // note: signal_row_changed() is emitted three times from here:
  row[columns.checked] = true;
  row[columns.comm]    = layer->get_data()->comm;
  row[columns.weight]  = Pango::WEIGHT_NORMAL;
  row[columns.layer]   = layer;
  row[columns.data]    = data;
}

void
PanelTRK::get_data(geo_data & world, bool visible) const {
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
     if (visible && !(*i)[columns.checked]) continue;
     boost::shared_ptr<GObjTRK> layer = (*i)[columns.layer];
     world.trks.push_back(*(layer->get_data()));
  }
}

std::map<GObjTRK*, std::vector<int> >
PanelTRK::find_tpts(const iRect & r) const {
  std::map<GObjTRK*, std::vector<int> > ret;
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    if (!(*i)[columns.checked]) continue;
    boost::shared_ptr<GObjTRK> gobj = (*i)[columns.layer];
    std::vector<int> pts = gobj->find_trackpoints(r);
    if (pts.size()>0)
      ret.insert(pair<GObjTRK*, std::vector<int> >(gobj.get(), pts));
  }
  return ret;
}

int
PanelTRK::find_tpt(const iPoint & p, GObjTRK ** gobj,
             const bool segment, int radius) const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    if (!(*i)[columns.checked]) continue;
    boost::shared_ptr<GObjTRK> current_gobj = (*i)[columns.layer];
    *gobj = current_gobj.get();
    int d;
    if (segment) d = current_gobj->find_track(p, radius);
    else d = current_gobj->find_trackpoint(p, radius);
    if (d >= 0) return d;
  }
  *gobj = NULL;
  return -1;
}

bool
PanelTRK::upd_comm(GObjTRK * sel_gobj, bool dir){
  bool ret=false;
  Gtk::TreeNodeChildren::const_iterator i;
  for (i = store->children().begin();
       i != store->children().end(); i++){
    string comm = (*i)[columns.comm];
    boost::shared_ptr<GObjTRK> layer = (*i)[columns.layer];
    if (!layer) continue;
    // select layer if sel_gobj!=NULL
    if (sel_gobj && sel_gobj!=layer.get()) continue;
    if (comm!=layer->get_data()->comm){
      if (dir) layer->get_data()->comm = comm;
      else (*i)[columns.comm] = layer->get_data()->comm;
      ret = true;
    }
  }
  return ret;
}
