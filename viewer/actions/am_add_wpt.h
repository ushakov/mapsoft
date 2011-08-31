#ifndef AM_ADD_WPT_H
#define AM_ADD_WPT_H

#include "action_mode.h"
#include <sstream>
#include <iomanip>

class AddWaypoint : public ActionMode {
public:
    AddWaypoint (Mapview * mapview) : ActionMode(mapview) {
      builder = Gtk::Builder::create_from_file(
        "/usr/share/mapsoft/dialogs/add_wpt.xml");
      GETW("add_wpt", dlg)
      GETW("name", name)
      GETW("comm", comm)
      GETW("lonlat", lonlat)
      GETW("font_size", font_size)
      GETW("fg", fg)
      GETW("bg", bg)
      GETW("ok", ok)
      GETW("cancel", cancel)
      ok->signal_clicked().connect(
          sigc::mem_fun (this, &AddWaypoint::on_ok));
      cancel->signal_clicked().connect(
          sigc::mem_fun(this, &AddWaypoint::abort));
      dlg->signal_delete_event().connect_notify(
         sigc::hide(sigc::mem_fun(this, &AddWaypoint::abort)));
    } 
    ~AddWaypoint(){
      delete dlg, name, comm, lonlat, fg, bg, font_size, ok, cancel;
    }


    std::string get_name() {
	return "Add Waypoint";
    }

    void abort() {
      mapview->statusbar.push("",0);
      mapview->rubber.clear();
      dlg->hide();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    void handle_click(iPoint p, const Gdk::ModifierType & state) {
         if (!mapview->have_reference){
           mapview->statusbar.push("No geo-reference", 0);
           return;
         }
        g_map map = mapview->reference;
        convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));
        g_waypoint wpt;
        wpt.x = p.x; wpt.y=p.y;
	cnv.frw(wpt);
        wpt2dlg(wpt);
        dlg->show();
        mapview->rubber.clear();
        mapview->rubber.add_src_mark(p);
    }

private:

    Glib::RefPtr<Gtk::Builder> builder;
    Gtk::Dialog *dlg;
    Gtk::ColorButton *fg, *bg;
    Gtk::Entry *name, *comm, *lonlat;
    Gtk::SpinButton *font_size;
    Gtk::Button *ok, *cancel;

    void on_ok(){
      g_waypoint wpt;
      dlg2wpt(wpt);

      // try to find active wpt layer
      for (int i=0; i<mapview->wpt_layers.size(); i++){
        LayerWPT * layer = dynamic_cast<LayerWPT *>
           (mapview->wpt_layers[i].get());
        if (mapview->workplane.get_layer_active(layer)) {
          layer->get_world()->wpts[0].push_back(wpt);
          mapview->workplane.refresh_layer(layer);
          return;
        }
      }
     // if there is no active wpt layer
      boost::shared_ptr<geo_data> world(new geo_data);
      g_waypoint_list wpts;
      wpts.push_back(wpt);
      world->wpts.push_back(wpts);
      mapview->add_world(world, "new", false);
      abort();
    }

    void dlg2wpt(g_waypoint &wpt){
      dPoint p = boost::lexical_cast<dPoint>(lonlat->get_text());
      wpt.x=p.x; wpt.y=p.y;
      wpt.name = name->get_text();
      wpt.comm = comm->get_text();
      wpt.font_size = (int)font_size->get_value();
      Gdk::Color c = fg->get_color();
      wpt.color.value=
        (((unsigned)c.get_red()   & 0xFF00) >> 8) +
         ((unsigned)c.get_green() & 0xFF00) +
        (((unsigned)c.get_blue()  & 0xFF00) << 8);
      c = bg->get_color();
      wpt.bgcolor.value=
        (((unsigned)c.get_red()   & 0xFF00) >> 8) +
         ((unsigned)c.get_green() & 0xFF00) +
        (((unsigned)c.get_blue()  & 0xFF00) << 8);
    }
    void wpt2dlg(const g_waypoint &wpt){
      std::ostringstream ws;
      ws.setf(std::ios::fixed);
      ws << std::setprecision(6) <<wpt.x << ", " << wpt.y;
      lonlat->set_text(ws.str());
      name->set_text(wpt.name);
      comm->set_text(wpt.comm);
      font_size->set_value(wpt.font_size);
      Gdk::Color c;
      c.set_rgb((wpt.color.value & 0xFF)<<8,
                (wpt.color.value & 0xFF00),
                (wpt.color.value & 0xFF0000)>>8);
      fg->set_color(c);
      c.set_rgb((wpt.bgcolor.value & 0xFF)<<8,
                (wpt.bgcolor.value & 0xFF00),
                (wpt.bgcolor.value & 0xFF0000)>>8);
      bg->set_color(c);
    }
};

#endif /* AM_ADD_WPT_H */
