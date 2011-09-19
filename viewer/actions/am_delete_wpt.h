#ifndef AM_DELETE_WPT_H
#define AM_DELETE_WPT_H

#include "action_mode.h"
#include <sstream>

class DeleteDialog : public Gtk::Dialog{
    Gtk::Label * label;
  public:
    DeleteDialog(){
      add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
      add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
      set_title("Delete Waypoint");

      label = manage(new Gtk::Label("", 0.0, 0.5));
      label->set_padding(15,10);
      label->set_alignment(Gtk::ALIGN_LEFT);
      get_vbox()->add(*label);
    }

    void set_name(const std::string & name){
      assert(label);
      std::ostringstream s;
      s << "Delete waypoint <b>" << name << "</b>?";
      label->set_markup(s.str());
    }
};

class DeleteWaypoint : public ActionMode {
public:
    DeleteWaypoint (Mapview * mapview) : ActionMode(mapview) {
      dlg.signal_response().connect(
        sigc::mem_fun (this, &DeleteWaypoint::on_result));
    }

    std::string get_name() { return "Delete Waypoint"; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      pt_num=find_wpt(p, &layer);
      if (pt_num < 0) return;
      dlg.set_name(layer->get_pt(pt_num)->name);
      dlg.show_all();
    }

private:
    int pt_num;
    LayerWPT * layer;
    DeleteDialog dlg;

    void on_result(int r) {
      dlg.hide_all();
      if ((r != Gtk::RESPONSE_OK) || (pt_num<0)) return;
      layer->get_data()->erase(layer->get_data()->begin() + pt_num);
      mapview->workplane.refresh_layer(layer);
    }
};

#endif /* AM_DELETE_WPT_H */
