#ifndef AM_ADD_TPT_H
#define AM_ADD_TPT_H

#include "action_mode.h"
#include "../dialogs/trk_pt.h"

class AddTrackpoint : public ActionMode {
public:
    AddTrackpoint (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Add Trackpoint"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::ADD; }

    void activate() {
      abort();
    }

    void abort() {
      mapview->rubber.clear();
      gobj = 0;
      mystate = 0;
    }


    void handle_click(iPoint p, const Gdk::ModifierType & state) {

        if (mystate==0){ // select point

          pt_num = mapview->panel_trks.find_tpt(p, &gobj, true);
          if (pt_num < 0) return;

          // we need point after insert place 0..size()
          pt_num++;

          // check for edge points
          GObjTRK * gobj1 = NULL;
          int pt_num1 = mapview->panel_trks.find_tpt(p, &gobj1, false);
          edge = 0;
          if (gobj == gobj1){
            if (pt_num1 == 0){
              pt_num = pt_num1;
              edge = 1;
            }
            else if (pt_num1 == gobj->get_data()->size()-1){
              pt_num = pt_num1+1;
              edge = 2;
            }
            else if (gobj->get_pt(pt_num1)->start){
              pt_num = pt_num1;
              edge = 1;
            }
            else if (gobj->get_pt(pt_num1+1)->start){
              pt_num = pt_num1+1;
              edge = 2;
            }
          }
          else if ((pt_num<gobj->get_data()->size()) &&
                   (gobj->get_pt(pt_num)->start)) return; // don't add points it track gaps

          if (edge != 1){
            dPoint p1 = *gobj->get_pt(pt_num-1);
            gobj->get_cnv()->bck(p1);
            mapview->rubber.add_line(p1);
          }
          if (edge != 2){
            dPoint p1 = *gobj->get_pt(pt_num);
            gobj->get_cnv()->bck(p1);
            mapview->rubber.add_line(p1);
          }
          mystate=1;

        } else { // add point
          if (!gobj) return;
          dPoint pt(p);
          gobj->get_cnv()->frw(pt);

          g_trackpoint tpt;
          tpt.start=false;
          tpt.dPoint::operator=(pt);

          if (edge == 1){
            tpt.start = true;
            gobj->get_pt(pt_num)->start = false;
          }
          gobj->get_data()->insert(gobj->get_data()->begin()+pt_num, tpt);
          mapview->set_changed();
          mapview->panel_trks.refresh_gobj(gobj);
          abort();
        }
    }

private:
    int pt_num, edge;
    GObjTRK * gobj;
    int mystate; // 0 - select point, 1 - move point
};

#endif /* AM_ADD_TPT_H */
