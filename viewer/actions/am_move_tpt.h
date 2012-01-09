#ifndef AM_MOVE_TPT_H
#define AM_MOVE_TPT_H

#include "action_mode.h"

class MoveTrackpoint : public ActionMode {
public:
    MoveTrackpoint (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Move Trackpoint"; }

    void activate() {
      abort();
    }

    void abort() {
      mapview->rubber.clear();
      tpt = 0;
      layer = 0;
      mystate = 0;
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {

        if (mystate==0){ // select point
          int pt_num = mapview->find_tpt(p, &layer);
          if (pt_num < 0) return;
          tpt = layer->get_pt(pt_num);

          convs::map2pt cnv(layer->get_cnv());

          if ((pt_num > 0)&&(!tpt->start)){
            dPoint p1 = *layer->get_pt(pt_num-1);
            cnv.bck(p1);
            mapview->rubber.add_line(p1);
          }
          if ((pt_num < layer->get_data()->size() - 1)&&
              (!layer->get_pt(pt_num+1)->start)){
            dPoint p1 = *layer->get_pt(pt_num+1);
            cnv.bck(p1);
            mapview->rubber.add_line(p1);
          }
          if (mapview->rubber.size()==0){
            mapview->rubber.add_line(p);
          }
          mystate=1;
        } else { // move point
          if (!tpt) return;
          convs::map2pt cnv(layer->get_cnv());
          dPoint pt(p);
          cnv.frw(pt);
          tpt->dPoint::operator=(pt);
          mapview->workplane.refresh_layer(layer);
          abort();
        }
    }

private:
    g_trackpoint * tpt;
    LayerTRK * layer;
    int mystate; // 0 - select point, 1 - move point
};

#endif /* AM_MOVE_TPT_H */
