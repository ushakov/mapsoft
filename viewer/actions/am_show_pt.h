#ifndef AM_SHOW_PTH
#define AM_SHOW_PTH

#include "action_mode.h"
#include <geo/geo_nom.h>
#include "../widgets.h"

class ShowPt : public ActionMode {
public:
    ShowPt (Mapview * mapview) : ActionMode(mapview) {
      builder = Gtk::Builder::create_from_file(
        "/usr/share/mapsoft/dialogs/show_pt.xml");
      GETW("show_pt", dlg)
      GETW("cancel", cancel)
      GETWD("coord_box", coord);
      GETWD("nom_box", nom);

      coord->signal_jump().connect(
          sigc::mem_fun (this, &ShowPt::on_jump));
      nom->signal_jump().connect(
          sigc::mem_fun (this, &ShowPt::on_jump_to_map));
      cancel->signal_clicked().connect(
          sigc::mem_fun(this, &ShowPt::abort));
      dlg->signal_delete_event().connect_notify(
         sigc::hide(sigc::mem_fun(this, &ShowPt::abort)));
      dlg->set_title(get_name());
    }
    ~ShowPt(){
      delete dlg;
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Show point information";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {dlg->hide();}

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) {
      if (!mapview->have_reference){
        mapview->statusbar.push("No geo-reference", 0);
        return;
      }

      convs::map2pt cnv(mapview->reference,
        Datum("wgs84"), Proj("lonlat"), Options());

      dPoint pt(p);
      cnv.frw(pt);

      mapview->rubber.clear();
      mapview->rubber.add_src_mark(p);
      dlg->show_all();
      coord->set_ll(dPoint(pt));
      nom->set_ll(dPoint(pt));
    }

    void on_jump_to_map(dPoint p){
      coord->set_ll(p);
      on_jump(p);
    }

    void on_jump(dPoint p){
      convs::map2pt cnv(mapview->reference,
        Datum("wgs84"), Proj("lonlat"), Options());
      cnv.bck(p);
      mapview->rubber.clear();
      mapview->viewer.set_center(p);
      mapview->rubber.add_src_mark(p);
    }


private:
    Glib::RefPtr<Gtk::Builder> builder;
    Gtk::Dialog *dlg;
    CoordBox *coord;
    NomBox   *nom;
    Gtk::Button *cancel;
};

#endif /* AM_SHOW_PTH */
