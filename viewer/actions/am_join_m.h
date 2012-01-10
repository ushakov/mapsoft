#ifndef AM_JOIN_VIS_MAP
#define AM_JOIN_VIS_MAP

#include "action_mode.h"

// join visible maps action

class JoinVisMap : public ActionMode{
public:
    JoinVisMap (Mapview * mapview) :  ActionMode(mapview) { }

    std::string get_name() { return "Join visible maps"; }
    bool is_radio() { return false; }

    void activate() {
      Gtk::TreeNodeChildren::const_iterator i;
      boost::shared_ptr<g_map_list> newd(new g_map_list);
      i = mapview->map_ll.store->children().begin();
      while (i != mapview->map_ll.store->children().end()){
        if (!(*i)[mapview->map_ll.columns.checked]) {
          i++;
          continue;
        }
        boost::shared_ptr<LayerGeoMap> current_layer =
          (*i)[mapview->map_ll.columns.layer];
        g_map_list * curr = current_layer->get_data();
        if (!curr){
          i++;
          continue;
        }
        newd->insert(newd->end(), curr->begin(), curr->end());
        if (newd->size()) newd->comm = "JOIN";
        else newd->comm = curr->comm;
        mapview->workplane.remove_layer(
          i->get_value(mapview->map_ll.columns.layer).get());
        i = mapview->map_ll.store->erase(i);
      }
      if (newd->size()) mapview->add_maps(newd);
    }
};

#endif /* AM_ADD_FILE */
