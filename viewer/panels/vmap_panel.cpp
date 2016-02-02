#include "vmap_panel.h"
#include "../mapview.h"

void
PanelVMAP::add(const boost::shared_ptr<vmap::world> data) {
  // note correct order:
  // - put layer to the workplane
  // - set layer/or mapview ref (layer ref is set through workplane)
  // - put layer to LayerList (panel_edited call, workplane refresh)
  // depth is set to DEPTH_DATA0 to evoke refresh!
  opts.put("transp_margins", true);
  boost::shared_ptr<GObjVMAP> layer(new GObjVMAP(data.get(), opts));
  add_gobj(layer.get(), 0);
//  if (!mapview->have_reference)
//    mapview->set_ref(layer->get_myref());
  Gtk::TreeModel::iterator it = store->append();
  Gtk::TreeModel::Row row = *it;
  // note: signal_row_changed() is emitted three times from here:
  row[columns.checked] = true;
  row[columns.comm]    = layer->get_data()->name;
  row[columns.weight]  = Pango::WEIGHT_NORMAL;
  row[columns.layer]   = layer;
  row[columns.data]    = data;
}
