#ifndef AM_ADD_WPT_H
#define AM_ADD_WPT_H

#include "action_mode.h"
#include "../widgets.h"

class AddWaypoint : public ActionMode {
public:
    AddWaypoint (Mapview * mapview) : ActionMode(mapview) {
      builder = Gtk::Builder::create_from_file(
        "/usr/share/mapsoft/dialogs/add_wpt.xml");
      GETW("add_wpt", dlg)
      GETW("name", name)
      GETW("comm", comm)
      GETW("font_size", font_size)
      GETW("size", size)
      GETW("fg", fg)
      GETW("bg", bg)
      GETW("ok", ok)
      GETW("cancel", cancel)
      GETWD("coord", coord);

      coord->signal_jump().connect(
          sigc::mem_fun (this, &AddWaypoint::on_jump));

      ok->signal_clicked().connect(
          sigc::mem_fun (this, &AddWaypoint::on_ok));
      cancel->signal_clicked().connect(
          sigc::mem_fun(this, &AddWaypoint::abort));
      dlg->signal_delete_event().connect_notify(
         sigc::hide(sigc::mem_fun(this, &AddWaypoint::abort)));
      dlg->set_title(get_name());
      wpt2dlg();
    }
    ~AddWaypoint(){
      delete dlg;
    }


    std::string get_name() {
	return "Add Waypoint";
    }

    void abort() {
      mapview->rubber.clear();
      dlg->hide();
      wpt.name=wpt.comm="";
      wpt2dlg();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    void handle_click(iPoint p, const Gdk::ModifierType & state) {
         if (!mapview->have_reference){
           mapview->statusbar.push("No geo-reference", 0);
           return;
         }
        g_map map = mapview->reference;
        convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));
        wpt.x = p.x; wpt.y=p.y;
        cnv.frw(wpt);
        coord->set_ll(wpt);
        dlg->show_all();
        mapview->rubber.clear();
        mapview->rubber.add_src_mark(p);
    }

private:

    Glib::RefPtr<Gtk::Builder> builder;
    Gtk::Dialog *dlg;
    Gtk::ColorButton *fg, *bg;
    Gtk::Entry *name, *comm, *lonlat;
    Gtk::SpinButton *font_size, *size;
    Gtk::Button *ok, *cancel;
    CoordBox *coord;
    g_waypoint wpt;

    void on_ok(){
      dlg2wpt();

      // try to find active wpt layer
      LayerWPT * layer = find_wpt_layer();
      if (layer){
        g_waypoint_list * wpts = layer->get_data();
        wpts->push_back(wpt);
        mapview->workplane.refresh_layer(layer);
        abort();
        return;
      }
     // if there is no active wpt layer add one
      boost::shared_ptr<g_waypoint_list> data(new g_waypoint_list);
      data->push_back(wpt);
      data->comm="NEW";
      mapview->add_wpts(data);
      abort();
    }

    void on_jump(dPoint p){
      convs::map2pt cnv(mapview->reference,
        Datum("wgs84"), Proj("lonlat"), Options());
      cnv.bck(p);
      mapview->rubber.clear();
      mapview->viewer.set_center(p);
      mapview->rubber.add_src_mark(p);
    }

    void dlg2wpt(){
      dPoint p = coord->get_ll();
      wpt.x=p.x; wpt.y=p.y;
      wpt.name = name->get_text();
      wpt.comm = comm->get_text();
      wpt.font_size = (int)font_size->get_value();
      wpt.size = (int)size->get_value();
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
    void wpt2dlg(){
      coord->set_ll(wpt);
      name->set_text(wpt.name);
      comm->set_text(wpt.comm);
      font_size->set_value(wpt.font_size);
      size->set_value(wpt.size);
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
