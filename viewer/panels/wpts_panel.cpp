#include "wpts_panel.h"

PanelWPT::PanelWPT () {
  store = Gtk::ListStore::create(columns);
  set_model(store);
  append_column_editable("V", columns.checked);
  append_column_editable("Layer", columns.comm);
  set_enable_search(false);
  set_headers_visible(false);
  set_reorderable(false);
}

void
PanelWPT::add_gobj (const boost::shared_ptr<GObjWPT> layer,
                 const boost::shared_ptr<g_waypoint_list> data) {
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
PanelWPT::remove_gobj(GObjWPT * L){
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    boost::shared_ptr<GObjWPT> gobj = (*i)[columns.layer];
    if (gobj.get() != L) continue;
    store->erase(i);
    break;
  }
  Workplane::remove_gobj(L);
}

void
PanelWPT::remove_selected(){
  Gtk::TreeModel::iterator it = get_selection()->get_selected();
  if (!it) return;
  Workplane::remove_gobj(it->get_value(columns.layer).get());
  store->erase(it);
}

void
PanelWPT::get_data(geo_data & world, bool visible) const {
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
     if (visible && !(*i)[columns.checked]) continue;
     boost::shared_ptr<GObjWPT> layer = (*i)[columns.layer];
     world.wpts.push_back(*(layer->get_data()));
  }
}

GObjWPT *
PanelWPT::find_gobj() const {
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    if (!(*i)[columns.checked]) continue;
    boost::shared_ptr<GObjWPT> gobj = (*i)[columns.layer];
    return gobj.get();
  }
  return NULL;
}

int
PanelWPT::find_wpt(const iPoint & p, GObjWPT ** gobj, int radius) const {
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    if (!(*i)[columns.checked]) continue;
    boost::shared_ptr<GObjWPT> current_gobj = (*i)[columns.layer];
    *gobj = current_gobj.get();
    int d = current_gobj->find_waypoint(p, radius);
    if (d >= 0) return d;
  }
  *gobj = NULL;
  return -1;
}

std::map<GObjWPT*, std::vector<int> >
PanelWPT::find_wpts(const iRect & r) const {
  std::map<GObjWPT*, std::vector<int> > ret;
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    if (!(*i)[columns.checked]) continue;
    boost::shared_ptr<GObjWPT> gobj = (*i)[columns.layer];
    std::vector<int> pts = gobj->find_waypoints(r);
    if (pts.size()>0)
      ret.insert(pair<GObjWPT*, std::vector<int> >(gobj.get(), pts));
  }
  return ret;
}

bool
PanelWPT::upd_wp (Workplane & wp, int & d) const {
  bool ret=false;
  Gtk::TreeNodeChildren::const_iterator i;
  for (i = store->children().begin();
       i != store->children().end(); i++){
    boost::shared_ptr<GObjWPT> layer = (*i)[columns.layer];
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
PanelWPT::upd_comm(GObjWPT * sel_gobj, bool dir){
  bool ret=false;
  Gtk::TreeNodeChildren::const_iterator i;
  for (i = store->children().begin();
       i != store->children().end(); i++){
            string comm = (*i)[columns.comm];
    boost::shared_ptr<GObjWPT> layer = (*i)[columns.layer];
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
