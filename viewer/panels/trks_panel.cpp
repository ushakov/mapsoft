#include "trks_panel.h"

PanelTRK::PanelTRK () {
  store = Gtk::ListStore::create(columns);
  set_model(store);
  append_column_editable("V", columns.checked);
  append_column_editable("Layer", columns.comm);
  set_enable_search(false);
  set_headers_visible(false);
  set_reorderable(false);
}

void
PanelTRK::add_gobj (const boost::shared_ptr<GObjTRK> layer,
                 const boost::shared_ptr<g_track> data) {
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
PanelTRK::remove_gobj (GObjTRK * L){
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    boost::shared_ptr<GObjTRK> gobj = (*i)[columns.layer];
    if (gobj.get() != L) continue;
    store->erase(i);
    break;
  }
  Workplane::remove_gobj(L);
}

void
PanelTRK::remove_selected(){
  Gtk::TreeModel::iterator it = get_selection()->get_selected();
  if (!it) return;
  Workplane::remove_gobj(it->get_value(columns.layer).get());
  store->erase(it);
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

GObjTRK *
PanelTRK::find_gobj() const {
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    if (!(*i)[columns.checked]) continue;
    boost::shared_ptr<GObjTRK> gobj = (*i)[columns.layer];
    return gobj.get();
  }
  return NULL;
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
PanelTRK::upd_wp (Workplane & wp, int & d) const {
  bool ret=false;
  Gtk::TreeNodeChildren::const_iterator i;
  for (i = store->children().begin();
       i != store->children().end(); i++){
    boost::shared_ptr<GObjTRK> layer = (*i)[columns.layer];
    if (!layer) continue;
    // update visibility
    bool act = (*i)[columns.checked];
    if (wp.get_gobj_active(layer.get()) != act){
      wp.set_gobj_active(layer.get(), act);
      ret = true;
    }
    // update depth
    if (wp.get_gobj_depth(layer.get()) != d){
      wp.set_gobj_depth(layer.get(), d);
      ret = true;
    }
    d++;
  }
  return ret;
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
