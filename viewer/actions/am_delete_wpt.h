#ifndef AM_DELETE_WPT_H
#define AM_DELETE_WPT_H

#include "action_mode.h"
#include <sstream>

class DeleteWaypoint : public ActionMode {
public:
    DeleteWaypoint (Mapview * mapview) : ActionMode(mapview) {  }

    std::string get_name() { return "Delete Waypoint"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::DELETE; }

    void activate() { selection = false;}

    void abort() { activate(); }

    void del_pt(const iPoint & p){
      GObjWPT * layer;
      int n = mapview->find_wpt(p, &layer);
      if (n < 0) return;
      g_waypoint_list * wpts = layer->get_data();
      wpts->erase(wpts->begin()+n);
      if (wpts->size()==0){
        mapview->layer_wpts.gobj.remove_gobj(layer);
        mapview->layer_wpts.panel.remove_gobj(layer);
        mapview->refresh();
      }
      else
        mapview->layer_wpts.gobj.refresh_gobj(layer);
    }

    void del_pt(const iRect & r){
      std::map<GObjWPT*, std::vector<int> > pts=mapview->find_wpts(r);
      std::map<GObjWPT*, std::vector<int> >::iterator i;
      for (i=pts.begin(); i!=pts.end(); i++){
        std::vector<int> & nn = i->second;
        g_waypoint_list * wpts = i->first->get_data();
        for (int j=nn.size()-1; j>=0; j--)
          wpts->erase(wpts->begin()+nn[j]);
        if (wpts->size()==0){
          mapview->layer_wpts.gobj.remove_gobj(i->first);
          mapview->layer_wpts.panel.remove_gobj(i->first);
          mapview->refresh();
        }
        else
          mapview->layer_wpts.gobj.refresh_gobj(i->first);
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

#endif /* AM_DELETE_WPT_H */
