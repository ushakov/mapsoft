#ifndef VIEWER_WPTS_PANEL_H
#define VIEWER_WPTS_PANEL_H

#include "panel.h"
#include "img_io/gobj_wpt.h"

typedef LayerTabCols<GObjWPT, g_waypoint_list> PanelWPTCols;

/* Control panel for wpts layer. */

class Mapview;

class PanelWPT : public Panel<GObjWPT, g_waypoint_list> {
public:
    PanelWPT (Mapview * M): Panel(), mapview(M){}

    void add(const boost::shared_ptr<g_waypoint_list> data);

    void get_data(geo_data & world, bool visible) const;

    /* find waypoint in all gobjs */
    int find_wpt(const iPoint & p, GObjWPT ** gobj, int radius = 3) const;

    /* find waypoints in a rectangular area */
    std::map<GObjWPT*, std::vector<int> > find_wpts(const iRect & r) const;

    bool upd_comm(GObjWPT * sel_gobj=NULL, bool dir=true);

    Mapview * mapview;
};

#endif
