#ifndef AM_ADD_TPT_H
#define AM_ADD_TPT_H

#include "action_mode.h"
#include "../widgets.h"

class AddTrackpoint : public ActionMode {
public:
    AddTrackpoint (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Add Trackpoint"; }

    void activate() {
      abort();
    }

    void abort() {
      mapview->rubber.clear();
      layer = 0;
      mystate = 0;
    }


    void handle_click(iPoint p, const Gdk::ModifierType & state) {

        if (mystate==0){ // select point

          pt_num = mapview->find_tpt(p, &layer, true);
          if (pt_num < 0) return;

          // we need point after insert place 0..size()
          pt_num++;

          // check for edge points
          LayerTRK * layer1 = NULL;
          int pt_num1 = mapview->find_tpt(p, &layer1, false);
          edge = 0;
          if (layer == layer1){
            if (pt_num1 == 0){
              pt_num = pt_num1;
              edge = 1;
            }
            else if (pt_num1 == layer->get_data()->size()-1){
              pt_num = pt_num1+1;
              edge = 2;
            }
            else if (layer->get_pt(pt_num1)->start){
              pt_num = pt_num1;
              edge = 1;
            }
            else if (layer->get_pt(pt_num1+1)->start){
              pt_num = pt_num1+1;
              edge = 2;
            }
          }
          else if ((pt_num<layer->get_data()->size()) &&
                   (layer->get_pt(pt_num)->start)) return; // don't add points it track gaps

          convs::map2pt cnv(layer->get_cnv());

          if (edge != 1){
            dPoint p1 = *layer->get_pt(pt_num-1);
            cnv.bck(p1);
            mapview->rubber.add_line(p1);
          }
          if (edge != 2){
            dPoint p1 = *layer->get_pt(pt_num);
            cnv.bck(p1);
            mapview->rubber.add_line(p1);
          }
          mystate=1;

        } else { // move point
          if (!layer) return;
          convs::map2pt cnv(layer->get_cnv());
          dPoint pt(p);
          cnv.frw(pt);

          g_trackpoint tpt;
          tpt.start=false;
          tpt.dPoint::operator=(pt);

          if (edge == 1){
            tpt.start = true;
            layer->get_pt(pt_num)->start = false;
          }
          layer->get_data()->insert(layer->get_data()->begin()+pt_num, tpt);
          mapview->workplane.refresh_layer(layer);
          abort();
        }
    }

private:
    int pt_num, edge;
    LayerTRK * layer;
    int mystate; // 0 - select point, 1 - move point
};

#endif /* AM_ADD_TPT_H */
