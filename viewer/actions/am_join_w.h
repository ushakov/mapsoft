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
      i = mapview->panel_wpts.store->children().begin();
      while (i != mapview->panel_wpts.store->children().end()){
        if (!(*i)[mapview->panel_wpts.columns.checked]) {
          i++;
          continue;
        }

        boost::shared_ptr<GObjWPT> current_gobj =
          (*i)[mapview->panel_wpts.columns.layer];
        g_waypoint_list * curr = current_gobj->get_data();
        if (!curr){
          i++;
          continue;
        }
        newd->insert(newd->end(), curr->begin(), curr->end());
        if (newd->size()) newd->comm = "JOIN";
        else newd->comm = curr->comm;
        mapview->panel_wpts.remove_gobj(
          i->get_value(mapview->panel_wpts.columns.layer).get());
        i = mapview->panel_wpts.store->erase(i);
      }
      if (newd->size()) mapview->add_wpts(newd);
    }
};

#endif /* AM_ADD_FILE */
