#ifndef VIEWER_WPTS_PANEL_H
#define VIEWER_WPTS_PANEL_H

#include <gtkmm.h>
#include "img_io/gobj_wpt.h"

#include "workplane.h"
#include "panel_cols.h"

typedef LayerTabCols<GObjWPT, g_waypoint_list> PanelWPTCols;

/* Control panel for wpts layer. */

class PanelWPT : public Gtk::TreeView, public Workplane {
public:
    PanelWPT ();

    void add_gobj (const boost::shared_ptr<GObjWPT> layer,
                    const boost::shared_ptr<g_waypoint_list> data);

    void remove_gobj (GObjWPT * L);
    void remove_selected();

    void get_data(geo_data & world, bool visible) const;

    /* find first active object */
    GObjWPT * find_gobj() const;

    /* find waypoint in all gobjs */
    int find_wpt(const iPoint & p, GObjWPT ** gobj, int radius = 3) const;

    /* find waypoints in a rectangular area */
    std::map<GObjWPT*, std::vector<int> > find_wpts(const iRect & r) const;

    void clear() {store->clear(); Workplane::clear();}

    // Update workplane data (visibility and depth)
    //   d is a starting depth of layers
    // Return: change
    // Depth of last layer+1 returned in d
    bool upd_wp (Workplane & wp, int & d) const;

    bool upd_comm(GObjWPT * sel_gobj=NULL, bool dir=true);

    Glib::RefPtr<Gtk::ListStore> store;
    PanelWPTCols columns;
};

#endif
