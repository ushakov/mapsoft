#ifndef AM_EDIT_MAP_H
#define AM_EDIT_MAP_H

#include "action_mode.h"
#include "../widgets.h"

class EditMap : public ActionMode {
public:
    EditMap (Mapview * mapview) : ActionMode(mapview) { 
      dlg.signal_response().connect(
        sigc::mem_fun (this, &EditMap::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Edit Map"; }

    void abort() { dlg.hide_all(); }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      map_num=find_map(p, &layer);
      if (map_num < 0){ abort(); return; }
      dlg.map2dlg(layer->get_map(map_num));
      dlg.show_all();
    }

private:
    DlgMap dlg;
    int map_num;
    LayerGeoMap * layer;

    void on_result(int r) {
      if ((map_num<0) || (r!=Gtk::RESPONSE_OK)) return;
      dlg.dlg2map(layer->get_map(map_num));
      mapview->workplane.refresh_layer(layer);
    }

};

#endif /* AM_EDIT_MAP_H */
