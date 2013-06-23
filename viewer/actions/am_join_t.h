#ifndef AM_JOIN_VIS_TRK
#define AM_JOIN_VIS_TRK

#include "action_mode.h"

// join visible waypoints action

class JoinVisTrk : public ActionMode{
public:
    JoinVisTrk (Mapview * mapview) :  ActionMode(mapview) { }

    std::string get_name() { return "Join visible tracks"; }
    bool is_radio() { return false; }

    void activate() {
      Gtk::TreeNodeChildren::const_iterator i; 
      boost::shared_ptr<g_track> newd(new g_track);
      i = mapview->layer_trks.panel.store->children().begin();
      while (i != mapview->layer_trks.panel.store->children().end()){
        if (!(*i)[mapview->layer_trks.panel.columns.checked]) {
          i++;
          continue;
        }
        boost::shared_ptr<GObjTRK> current_gobj =
          (*i)[mapview->layer_trks.panel.columns.layer];
        g_track * curr = current_gobj->get_data();
        if (!curr){
          i++;
          continue;
        }
        newd->insert(newd->end(), curr->begin(), curr->end());
        if (newd->size()) newd->comm = "JOIN";
        else newd->comm = curr->comm;
        mapview->layer_trks.gobj.remove_gobj(
          i->get_value(mapview->layer_trks.panel.columns.layer).get());
        i = mapview->layer_trks.panel.store->erase(i);
      }
      if (newd->size()) mapview->add_trks(newd);
    }
};

#endif /* AM_ADD_FILE */
