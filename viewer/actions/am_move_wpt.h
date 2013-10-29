#ifndef AM_MOVE_WPT_H
#define AM_MOVE_WPT_H

#include "action_mode.h"

class MoveWaypoint : public ActionMode {
public:
    MoveWaypoint (Mapview * mapview_) : ActionMode(mapview_) { }

    std::string get_name() { return "Move Waypoint"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::GO_FORWARD; }

    void activate() {
      abort();
    }

    void abort() {
      wpt = 0;
      layer = 0;
      mystate=0;
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
        if (mystate==0){ // select point
          int pt_num=mapview->panel_wpts.find_wpt(p, &layer);
          if (pt_num < 0) return;

          wpt = layer->get_pt(pt_num);
          mapview->rubber.add_src_sq(p, 2);
          mapview->rubber.add_dst_sq(2);
          mapview->rubber.add_line(p);
          mystate=1;
        } else { // move point
          dPoint pt(p);
          layer->get_cnv()->frw(pt);
          wpt->dPoint::operator=(pt);
          mapview->set_changed();
          mapview->panel_wpts.refresh_gobj(layer);
          mapview->rubber.clear();
          abort();
        }
    }

private:
    g_waypoint * wpt;
    GObjWPT * layer;
    int mystate; // 0 - select point, 1 - move point
};

#endif /* AM_MOVE_WPT_H */
