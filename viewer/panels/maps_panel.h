#ifndef VIEWER_MAPS_PANEL_H
#define VIEWER_MAPS_PANEL_H

#include "panel.h"
#include "img_io/gobj_map.h"

typedef LayerTabCols<GObjMAP, g_map_list>      PanelMAPCols;

/* Control panel for maps layer. */

class Mapview;

class PanelMAP : public Panel<GObjMAP, g_map_list> {
public:
  PanelMAP (Mapview *M): mapview(M){}

  void add(const boost::shared_ptr<g_map_list> data);

  dPoint get_sel_point(){
    GObjMAP * O = find_selected();
    if (!O || !O->get_data()->size())
      return dPoint(nan(""), nan(""));
    return (*O->get_data())[0].center();
  }

  /* find top map in the point p */
  int find_map(const iPoint & p, GObjMAP ** gobj) const;

  bool upd_comm(GObjMAP * sel_gobj=NULL, bool dir=true);

  Mapview * mapview;
  Options opts;
};

#endif
