#ifndef AM_DELETE_TPT_H
#define AM_DELETE_TPT_H

#include "action_mode.h"

class DeleteTrackpoint : public ActionMode {
public:
    DeleteTrackpoint (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Delete Trackpoint"; }

    void activate() { selection = false;}

    void abort() { activate(); }

    void del_pt(g_track *track, int n){
      bool start = (*track)[n].start;
      track->erase(track->begin()+n);
      if (start && (n < track->size()))
        (*track)[n].start = start;
    }

    void del_pt(const iPoint & p){
      LayerTRK * layer;
      int n = mapview->find_tpt(p, &layer);
      if (n < 0) return;
      del_pt(layer->get_data(), n);
      if (layer->get_data()->size()==0)
        mapview->workplane.remove_layer(layer);
      else
        mapview->workplane.refresh_layer(layer);
    }

    void del_pt(const iRect & r){
      std::map<LayerTRK*, std::vector<int> > pts=mapview->find_tpts(r);
      std::map<LayerTRK*, std::vector<int> >::iterator i;
      for (i=pts.begin(); i!=pts.end(); i++){
        std::vector<int> & nn = i->second;
        for (int j=nn.size()-1; j>=0; j--)
          del_pt(i->first->get_data(), nn[j]);
        if (i->first->get_data()->size()==0)
          mapview->workplane.remove_layer(i->first);
        else
          mapview->workplane.refresh_layer(i->first);
      }
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      if (!selection){
        if (!(state&Gdk::CONTROL_MASK)){ // delete one point
          del_pt(p);
        }
        else { // start selection
          mapview->rubber.clear();
          mapview->rubber.add_rect(p);
          p1=p;
          selection=true;
        }
      }
      else { // delete region
        mapview->rubber.clear();
        del_pt(iRect(p1,p));
        selection=false;
      }
    }
private:
  bool selection;
  iPoint p1;
};

#endif /* AM_DELETE_TPT_H */
