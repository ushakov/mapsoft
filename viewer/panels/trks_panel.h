#ifndef VIEWER_TRKS_PANEL_H
#define VIEWER_TRKS_PANEL_H

#include "panel.h"
#include "img_io/gobj_trk.h"

typedef LayerTabCols<GObjTRK, g_track>         PanelTRKCols;

/* Control panel for trks layer. */

class Mapview;

class PanelTRK : public Panel<GObjTRK, g_track> {
public:

    PanelTRK (Mapview * M): mapview(M){
      opts.put("trk_draw_dots", "");
      opts.put("trk_draw_arrows", "");
      opts.put("trk_draw_v1", 0);
      opts.put("trk_draw_v2", 10);
      opts.put("trk_draw_h1", 0);
      opts.put("trk_draw_h2", 1000);
    }

    void add(const boost::shared_ptr<g_track> data);

    dPoint get_sel_point(){
      GObjTRK * O = find_selected();
      if (!O || !O->get_data()->size())
        return dPoint(nan(""), nan(""));
      return (*O->get_data())[0];
    }

    /* find track points in a rectangular area */
    std::map<GObjTRK*, std::vector<int> > find_tpts(const iRect & r) const;

    /* find trackpoint in all gobjs */
    // segment=true: find track point, returns its number 0..size()-1
    // segment=true: find track segment, return its
    //               first point 0..size()-2
    int find_tpt(const iPoint & p, GObjTRK ** gobj,
                 const bool segment = false, int radius = 3) const;

    bool upd_comm(GObjTRK * sel_gobj=NULL, bool dir=true);

    Mapview * mapview;
};

#endif
