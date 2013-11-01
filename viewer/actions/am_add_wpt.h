#ifndef AM_ADD_WPT_H
#define AM_ADD_WPT_H

#include "action_mode.h"
#include "../dialogs/wpt.h"

class AddWaypoint : public ActionMode {
public:
    AddWaypoint (Mapview * mapview) : ActionMode(mapview) {
      dlg.set_transient_for(*mapview);
      dlg.signal_jump().connect(
          sigc::mem_fun (this, &AddWaypoint::on_jump));
      dlg.signal_response().connect(
        sigc::mem_fun (this, &AddWaypoint::on_result));
      dlg.set_title(get_name());
      abort();
    }

    std::string get_name() { return "Add Waypoint"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::ADD; }

    void abort() {
      mapview->rubber.clear();
      dlg.hide_all();
      wpt.name=wpt.comm="";
      wpt.clear_alt();
      dlg.wpt2dlg(&wpt);
    }

    // Sends user click. Coordinates are in layer_wpts.gobj's discrete system.
    void handle_click(iPoint p, const Gdk::ModifierType & state) {
         if (!mapview->have_reference){
           mapview->statusbar.push("No geo-reference", 0);
           return;
         }
        wpt.dPoint::operator=(p);
        mapview->get_cnv()->frw(wpt);
        dlg.set_ll(wpt);
        dlg.show_all();
        mapview->rubber.clear();
        mapview->rubber.add_src_mark(p);
    }

private:
    DlgWpt dlg;
    g_waypoint wpt;

    void on_result(int r){
      if (r == Gtk::RESPONSE_OK){
        dlg.dlg2wpt(&wpt);
        // try to find active wpt gobj
        GObjWPT * gobj = mapview->panel_wpts.find_first();
        if (gobj){
          g_waypoint_list * wpts = gobj->get_data();
          wpts->push_back(wpt);
          mapview->set_changed();
          mapview->panel_wpts.refresh_gobj(gobj);
          abort();
          return;
        }
        // if there is no active wpt gobj add one
        boost::shared_ptr<g_waypoint_list> data(new g_waypoint_list);
        data->push_back(wpt);
        data->comm="NEW";
        mapview->panel_wpts.add(data);
      }
      abort();
    }

    void on_jump(dPoint p){
      mapview->get_cnv()->bck(p);
      mapview->rubber.clear();
      mapview->viewer.set_center(p);
      mapview->rubber.add_src_mark(p);
    }
};

#endif /* AM_ADD_WPT_H */
