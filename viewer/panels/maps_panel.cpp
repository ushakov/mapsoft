#include "maps_panel.h"

PanelMAP::PanelMAP () {
  store = Gtk::ListStore::create(columns);
  set_model(store);
  append_column_editable("V", columns.checked);
  int comm_cell_n = append_column_editable("V", columns.comm);

  Gtk::TreeViewColumn* comm_column = get_column(comm_cell_n - 1);
  Gtk::CellRendererText* comm_cell =
    (Gtk::CellRendererText*)get_column_cell_renderer(comm_cell_n - 1);
  if (comm_column && comm_cell)
    comm_column->add_attribute(comm_cell->property_weight(), columns.weight);

  set_enable_search(false);
  set_headers_visible(false);
  set_reorderable(false);
}

void
PanelMAP::add_gobj (const boost::shared_ptr<GObjMAP> layer,
                 const boost::shared_ptr<g_map_list> data) {
  Gtk::TreeModel::iterator it = store->append();
  Gtk::TreeModel::Row row = *it;
  row[columns.checked] = true;
  row[columns.layer]   = layer;
  row[columns.data]    = data;
  if (layer->get_data()->size() == 1){
    row[columns.comm] = (*layer->get_data())[0].comm;
    row[columns.weight] = Pango::WEIGHT_NORMAL;
  }
  else{
    row[columns.comm] = layer->get_data()->comm;
    row[columns.weight] = Pango::WEIGHT_BOLD;
  }
}

void
PanelMAP::remove_gobj (GObjMAP * L){
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    boost::shared_ptr<GObjMAP> gobj = (*i)[columns.layer];
    if (gobj.get() != L) continue;
    store->erase(i);
    break;
  }
  Workplane::remove_gobj(L);
}
void
PanelMAP::remove_selected(){
  Gtk::TreeModel::iterator it = get_selection()->get_selected();
  if (!it) return;
  Workplane::remove_gobj(it->get_value(columns.layer).get());
  store->erase(it);
}

void
PanelMAP::get_data(geo_data & world, bool visible) const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    if (visible && !(*i)[columns.checked]) continue;
    boost::shared_ptr<GObjMAP> layer = (*i)[columns.layer];
    world.maps.push_back(*(layer->get_data()));
  }
}

GObjMAP *
PanelMAP::find_gobj() const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    if (!(*i)[columns.checked]) continue;
    boost::shared_ptr<GObjMAP> gobj = (*i)[columns.layer];
    return gobj.get();
  }
  return NULL;
}

int
PanelMAP::find_map(const iPoint & p, GObjMAP ** gobj) const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = store->children().begin();
       i != store->children().end(); i++){
    if (!(*i)[columns.checked]) continue;
    boost::shared_ptr<GObjMAP> current_gobj = (*i)[columns.layer];
    *gobj = current_gobj.get();
    int d = current_gobj->find_map(p);
    if (d >= 0) return d;
  }
  *gobj = NULL;
  return -1;
}

bool
PanelMAP::upd_wp (Workplane & wp, int & d) const {
  bool ret=false;
  Gtk::TreeNodeChildren::const_iterator i;
  for (i = store->children().begin();
       i != store->children().end(); i++){
    boost::shared_ptr<GObjMAP> layer = (*i)[columns.layer];
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
PanelMAP::upd_comm(GObjMAP * sel_gobj, bool dir){
  bool ret=false;
  Gtk::TreeNodeChildren::const_iterator i;
  for (i = store->children().begin();
       i != store->children().end(); i++){
    boost::shared_ptr<GObjMAP> layer = (*i)[columns.layer];
    if (!layer) continue;
    // select layer if sel_gobj!=NULL
    if (sel_gobj && sel_gobj!=layer.get()) continue;
    // update comment
    string comm = (*i)[columns.comm];
    g_map_list * ml = layer->get_data();
    if (ml->size()==1){
      if (comm!=(*ml)[0].comm){
        if (dir) (*ml)[0].comm = comm;
        else (*i)[columns.comm] = (*ml)[0].comm;
        ret = true;
      }
    }
    else {
      if (comm!=ml->comm){
        if (dir) ml->comm = comm;
        else (*i)[columns.comm] = ml->comm;
        ret = true;
      }
    }
  }
  return ret;
}

