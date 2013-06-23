#ifndef AM_DELETE_TPT_H
#define AM_DELETE_TPT_H

#include "action_mode.h"

class DeleteTrackpoint : public ActionMode {
public:
    DeleteTrackpoint (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Delete Trackpoint"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::DELETE; }

    void activate() { selection = false;}

    void abort() { activate(); }

    void del_pt(g_track *track, int n){
      bool start = (*track)[n].start;
      track->erase(track->begin()+n);
      if (start && (n < track->size()))
        (*track)[n].start = start;
    }

    void del_pt(const iPoint & p){
      GObjTRK * gobj;
      int n = mapview->layer_trks.panel.find_tpt(p, &gobj);
      if (n < 0) return;
      del_pt(gobj->get_data(), n);
      if (gobj->get_data()->size()==0){
        mapview->layer_trks.gobj.remove_gobj(gobj);
        mapview->layer_trks.panel.remove_gobj(gobj);
        mapview->refresh();
      }
      else
        mapview->layer_trks.gobj.refresh_gobj(gobj);
    }

    void del_pt(const iRect & r){
      std::map<GObjTRK*, std::vector<int> > pts = mapview->layer_trks.panel.find_tpts(r);
      std::map<GObjTRK*, std::vector<int> >::iterator i;
      for (i=pts.begin(); i!=pts.end(); i++){
        std::vector<int> & nn = i->second;
        for (int j=nn.size()-1; j>=0; j--)
          del_pt(i->first->get_data(), nn[j]);
        if (i->first->get_data()->size()==0){
          mapview->layer_trks.gobj.remove_gobj(i->first);
          mapview->layer_trks.panel.remove_gobj(i->first);
          mapview->refresh();
        }
        else
          mapview->layer_trks.gobj.refresh_gobj(i->first);
      }
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      if (!selection){
        if (!(state&Gdk::CONTROL_MASK)){ // delete one point
          del_pt(p);
          mapview->set_changed();
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
        mapview->set_changed();
        selection=false;
      }
    }
private:
  bool selection;
  iPoint p1;
};

#endif /* AM_DELETE_TPT_H */
