#ifndef VIEWER_TRKS_PANEL_H
#define VIEWER_TRKS_PANEL_H

#include <gtkmm.h>
#include "img_io/gobj_trk.h"

#include "../workplane.h"
#include "panel_cols.h"

typedef LayerTabCols<GObjTRK, g_track>         TrkLLCols;

/* Control panel for trks layer. */

class TrkLL : public Gtk::TreeView {
public:
    Glib::RefPtr<Gtk::ListStore> store;
    TrkLLCols columns;

    TrkLL ();

    void add_gobj (const boost::shared_ptr<GObjTRK> layer,
                    const boost::shared_ptr<g_track> data);

    void remove_gobj (const GObjTRK * L);

    void get_data(geo_data & world, bool visible) const;

    /* find first active object */
    GObjTRK * find_gobj() const;

    /* find track points in a rectangular area */
    std::map<GObjTRK*, std::vector<int> > find_tpts(const iRect & r) const;

    /* find trackpoint in all gobjs */
    // segment=true: find track point, returns its number 0..size()-1
    // segment=true: find track segment, return its
    //               first point 0..size()-2
    int find_tpt(const iPoint & p, GObjTRK ** gobj,
                 const bool segment = false, int radius = 3) const;

    void clear() {store->clear();}

    bool upd_wp (Workplane & wp, int & d) const;

    bool upd_comm(GObjTRK * sel_gobj=NULL, bool dir=true);
};

#endif