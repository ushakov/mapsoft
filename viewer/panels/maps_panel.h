#ifndef VIEWER_MAPS_PANEL_H
#define VIEWER_MAPS_PANEL_H

#include "panel.h"
#include "img_io/gobj_map.h"

typedef LayerTabCols<GObjMAP, g_map_list>      PanelMAPCols;

/* Control panel for maps layer. */

class Mapview;

class PanelMAP : public Panel<GObjMAP, g_map_list> {
public:
  PanelMAP (Mapview *M): Panel(), mapview(M){}

  void add(const boost::shared_ptr<g_map_list> data);

  void get_data(geo_data & world, bool visible) const;

  /* find top map in the point p */
  int find_map(const iPoint & p, GObjMAP ** gobj) const;

  bool upd_comm(GObjMAP * sel_gobj=NULL, bool dir=true);

  Mapview * mapview;
};

#endif
