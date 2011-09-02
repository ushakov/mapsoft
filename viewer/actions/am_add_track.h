#ifndef AM_ADD_TRACK_H
#define AM_ADD_TRACK_H

#include <sstream>
#include <iomanip>
#include "action_mode.h"

class AddTrack : public ActionMode {
public:
    AddTrack (Mapview * mapview) : ActionMode(mapview) {
      builder = Gtk::Builder::create_from_file(
        "/usr/share/mapsoft/dialogs/add_trk.xml");
      GETW("add_trk", dlg)
      GETW("comm", comm)
      GETW("width", width)
      GETW("fg", fg)
      GETW("ok", ok)
      GETW("info", info)
      GETW("cancel", cancel)
      ok->signal_clicked().connect(
          sigc::mem_fun (this, &AddTrack::on_ok));
      cancel->signal_clicked().connect(
          sigc::mem_fun(this, &AddTrack::abort));
      dlg->signal_delete_event().connect_notify(
          sigc::hide(sigc::mem_fun(this, &AddTrack::abort)));
      dlg->set_title(get_name());
    }
    ~AddTrack(){
      delete dlg;
    }


    std::string get_name() { return "Add Track"; }
    void activate() { abort(); }
    void abort() {
      mapview->statusbar.push("",0);
      new_track.clear();
      new_track.comm="";
      mapview->rubber.clear();
      dlg->hide();
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
         if (!mapview->have_reference){
           mapview->statusbar.push("No geo-reference", 0);
           return;
         }

         if (new_track.size() == 0){
           trk2dlg();
           dlg->show();
         }

        if (state&Gdk::CONTROL_MASK){ // remove point
          if (new_track.size()>0) new_track.pop_back();
          if (mapview->rubber.size()>0){
            mapview->rubber.pop();
          }
          if (mapview->rubber.size()>0){
            RubberSegment s = mapview->rubber.pop();
            s.flags |= RUBBFL_MOUSE_P2;
            s.p2=iPoint(0,0);
            mapview->rubber.add(s);
          }
        }
        else{ // add point
          if (mapview->rubber.size()>0){
            RubberSegment s = mapview->rubber.pop();
            s.flags &= ~RUBBFL_MOUSE;
            s.p2 = p;
            mapview->rubber.add(s);
          }
          mapview->rubber.add_diag(p);
          g_map map = mapview->reference;
          convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));

          g_trackpoint pt;
          pt.x = p.x; pt.y=p.y;
	  cnv.frw(pt);
	  new_track.push_back(pt);
        }

	std::ostringstream st;
	st << "Points: <b>"
           << new_track.size() << "</b>, Length: <b>"
           << std::setprecision(2) << std::fixed
           << new_track.length()/1000 << "</b> km";
        info->set_markup(st.str());
    }

private:
    g_track  new_track;

    Glib::RefPtr<Gtk::Builder> builder;
    Gtk::Dialog *dlg;
    Gtk::Entry *comm;
    Gtk::SpinButton *width;
    Gtk::ColorButton *fg;
    Gtk::Label *info;
    Gtk::Button *ok, *cancel;

    void on_ok(){
      dlg2trk();
      boost::shared_ptr<geo_data> world(new geo_data);
      world->trks.push_back(new_track);
      mapview->add_world(world, new_track.comm, false);
      abort();
    }

    void dlg2trk(){
      new_track.comm = comm->get_text();
      new_track.width = (int)width->get_value();
      Gdk::Color c = fg->get_color();
      new_track.color.value=
        (((unsigned)c.get_red()   & 0xFF00) >> 8) +
         ((unsigned)c.get_green() & 0xFF00) +
        (((unsigned)c.get_blue()  & 0xFF00) << 8);
    }
    void trk2dlg(){
      comm->set_text(new_track.comm);
      width->set_value(new_track.width);
      Gdk::Color c;
      c.set_rgb((new_track.color.value & 0xFF)<<8,
                (new_track.color.value & 0xFF00),
                (new_track.color.value & 0xFF0000)>>8);
      fg->set_color(c);
    }
};

#endif /* AM_ADD_TRACK_H */
