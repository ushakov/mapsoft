#ifndef VIEWER_VMAP_PANEL_H
#define VIEWER_VMAP_PANEL_H

#include "panel.h"
#include "img_io/gobj_vmap.h"

typedef LayerTabCols<GObjVMAP, vmap::world> PanelVMAPCols;

/* Control panel for maps layer. */

class Mapview;

class PanelVMAP : public Panel<GObjVMAP, vmap::world> {
public:
  PanelVMAP (Mapview *M): mapview(M){}

  void add(const boost::shared_ptr<vmap::world> data);
  bool upd_comm(GObjVMAP * sel_gobj=NULL, bool dir=true){return true;}

  Mapview * mapview;
  Options opts;
};

#endif
