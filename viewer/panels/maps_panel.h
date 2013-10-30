#ifndef VIEWER_MAPS_PANEL_H
#define VIEWER_MAPS_PANEL_H

#include <gtkmm.h>
#include "img_io/gobj_map.h"

#include "workplane.h"
#include "panel_cols.h"

typedef LayerTabCols<GObjMAP, g_map_list>      PanelMAPCols;

/* Control panel for maps layer. */

class Mapview;

class PanelMAP : public Gtk::TreeView, public Workplane {
public:

  PanelMAP (Mapview *M);
  void add_gobj (const boost::shared_ptr<GObjMAP> layer,
                 const boost::shared_ptr<g_map_list> data);
  void remove_gobj (GObjMAP * L);
  void remove_selected();

  void get_data(geo_data & world, bool visible) const;

  /* find first active object */
  GObjMAP * find_gobj() const;

  /* find top map in the point p */
  int find_map(const iPoint & p, GObjMAP ** gobj) const;

  void clear() {store->clear(); Workplane::clear();}

  bool upd_wp (Workplane & wp, int & d) const;

  bool upd_comm(GObjMAP * sel_gobj=NULL, bool dir=true);

  Glib::RefPtr<Gtk::ListStore> store;
  PanelMAPCols columns;
  Glib::RefPtr<Gtk::UIManager> ui_manager;
  Mapview * mapview;
};

#endif
