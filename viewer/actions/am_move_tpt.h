#ifndef AM_MOVE_TPT_H
#define AM_MOVE_TPT_H

#include "action_mode.h"

class MoveTrackpoint : public ActionMode {
public:
    MoveTrackpoint (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Move Trackpoint"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::GO_FORWARD; }

    void activate() {
      abort();
    }

    void abort() {
      mapview->rubber.clear();
      tpt = 0;
      gobj = 0;
      mystate = 0;
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {

        if (mystate==0){ // select point
          int pt_num = mapview->find_tpt(p, &gobj);
          if (pt_num < 0) return;
          tpt = gobj->get_pt(pt_num);


          if ((pt_num > 0)&&(!tpt->start)){
            dPoint p1 = *gobj->get_pt(pt_num-1);
            gobj->get_cnv()->bck(p1);
            mapview->rubber.add_line(p1);
          }
          if ((pt_num < gobj->get_data()->size() - 1)&&
              (!gobj->get_pt(pt_num+1)->start)){
            dPoint p1 = *gobj->get_pt(pt_num+1);
            gobj->get_cnv()->bck(p1);
            mapview->rubber.add_line(p1);
          }
          if (mapview->rubber.size()==0){
            mapview->rubber.add_line(p);
          }
          mystate=1;
        } else { // move point
          if (!tpt) return;
          dPoint pt(p);
          gobj->get_cnv()->frw(pt);
          tpt->dPoint::operator=(pt);
          mapview->set_changed();
          mapview->layer_trks.gobj.refresh_gobj(gobj);
          abort();
        }
    }

private:
    g_trackpoint * tpt;
    GObjTRK * gobj;
    int mystate; // 0 - select point, 1 - move point
};

#endif /* AM_MOVE_TPT_H */
