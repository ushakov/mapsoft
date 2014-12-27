#include "maps_panel.h"
#include "../mapview.h"

void
PanelMAP::add(const boost::shared_ptr<g_map_list> data) {
  boost::shared_ptr<GObjMAP>
    layer(new GObjMAP(data.get(), opts));
  add_gobj(layer.get(), 0);
  mapview->set_ref(layer->get_myref());

  std::string comm = data->comm;
  Pango::Weight weight=Pango::WEIGHT_BOLD;

  if (data->size() == 1){
    comm=(*data)[0].comm;
    weight=Pango::WEIGHT_NORMAL;
  }

  Gtk::TreeModel::iterator it = store->append();
  Gtk::TreeModel::Row row = *it;
  row[columns.checked] = true;
  row[columns.layer]   = layer;
  row[columns.data]    = data;
  row[columns.comm]    = comm;
  row[columns.weight]  = weight;
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
  if (ret) signal_data_changed().emit();
  return ret;
}

