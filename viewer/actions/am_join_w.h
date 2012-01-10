#ifndef AM_JOIN_VIS_WPT
#define AM_JOIN_VIS_WPT

#include "action_mode.h"

// join visible waypoints action

class JoinVisWpt : public ActionMode{
public:
    JoinVisWpt (Mapview * mapview) :  ActionMode(mapview) { }

    std::string get_name() { return "Join visible waypoints"; }
    bool is_radio() { return false; }

    void activate() {
      Gtk::TreeNodeChildren::const_iterator i; 
      boost::shared_ptr<g_waypoint_list> newd(new g_waypoint_list);
      i = mapview->wpt_ll.store->children().begin();
      while (i != mapview->wpt_ll.store->children().end()){
        if (!(*i)[mapview->wpt_ll.columns.checked]) {
          i++;
          continue;
        }
        boost::shared_ptr<LayerWPT> current_layer =
          (*i)[mapview->wpt_ll.columns.layer];
        g_waypoint_list * curr = current_layer->get_data();
        if (!curr){
          i++;
          continue;
        }
        newd->insert(newd->end(), curr->begin(), curr->end());
        if (newd->size()) newd->comm = "JOIN";
        else newd->comm = curr->comm;
        mapview->workplane.remove_layer(
          i->get_value(mapview->wpt_ll.columns.layer).get());
        i = mapview->wpt_ll.store->erase(i);
      }
      if (newd->size()) mapview->add_wpts(newd);
    }
};

#endif /* AM_ADD_FILE */
