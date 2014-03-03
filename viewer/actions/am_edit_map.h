#ifndef AM_EDIT_MAP_H
#define AM_EDIT_MAP_H

#include "action_mode.h"
#include "../dialogs/map.h"

class EditMap : public ActionMode {
public:
    EditMap (Mapview * mapview) : ActionMode(mapview) { 
      dlg.set_transient_for(*mapview);
      dlg.signal_response().connect(
        sigc::mem_fun (this, &EditMap::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Edit Map"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::EDIT; }

    void abort() { reset(true); }

    void reset(bool redraw) {
      if (redraw)
        dlg.hide_all();
      if (mark_gobj){
        mapview->panel_maps.Workplane::remove_gobj(mark_gobj.get());
        if (redraw) mapview->viewer.redraw();
      }
      mark_gobj.reset();
      mark_data.reset();
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {

      // find map
      map_num=mapview->panel_maps.find_map(p, &layer);
      if (map_num < 0) return;
      g_map * m = layer->get_map(map_num); 
      reset(false);

      // create layer with marks
      mark_data = boost::shared_ptr<g_map_list>(new g_map_list);
      mark_data->push_back(*m);
      mark_gobj = boost::shared_ptr<GObjMAP>(
          new GObjMAP(mark_data.get()));
      mark_gobj->show_brd();
      mark_gobj->show_ref();
      mark_gobj->hide_map();
      mark_gobj->set_ref(*layer->get_ref());
      mapview->panel_maps.Workplane::add_gobj(mark_gobj.get(), 50);
      mapview->viewer.redraw();

      // show dialog
      dlg.map2dlg(m);
      dlg.show_all();
    }

private:
    DlgMap dlg;
    int map_num;
    GObjMAP * layer;
    boost::shared_ptr<g_map_list> mark_data;
    boost::shared_ptr<GObjMAP> mark_gobj;

    void on_result(int r) {
      reset(true);
      if ((map_num<0) || (r!=Gtk::RESPONSE_OK)) return;
      dlg.dlg2map(layer->get_map(map_num));
      mapview->set_changed();
      mapview->panel_maps.upd_comm(layer, false); // move comm to ll
      mapview->panel_maps.refresh_gobj(layer);
    }

};

#endif /* AM_EDIT_MAP_H */
