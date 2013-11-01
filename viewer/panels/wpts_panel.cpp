#include "wpts_panel.h"
#include "../mapview.h"

void
PanelWPT::add(const boost::shared_ptr<g_waypoint_list> data) {
  // note correct order:
  // - put layer to the workplane
  // - set layer/or mapview ref (layer ref is set through workplane)
  // - put layer to LayerList (panel_edited call, workplane refresh)
  // depth is set to DEPTH_DATA0 to evoke refresh!
  boost::shared_ptr<GObjWPT> layer(new GObjWPT(data.get(), opts));
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
