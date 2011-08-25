#ifndef AM_SHOW_PTH
#define AM_SHOW_PTH

#include "action_mode.h"
#include "../generic_dialog.h"

#include <geo/geo_nom.h>


#define DLG_ROWS 9

class ShowPtDlg : public Gtk::Dialog{
  Gtk::Table table;
  Gtk::Label *l[DLG_ROWS];
  Gtk::Entry *ex[DLG_ROWS], *ey[DLG_ROWS];


public:
  ShowPtDlg(){
    set_title("Point information");
    table.resize(3, DLG_ROWS); // rows,cols
    table.set_spacings(2);
    const char *label_text[] = {
      "Workplain coordinates (px)",
      "WGS84 LatLong (degrees)",
      "Pulkovo LatLong (degrees)",
      "WGS84 tmerc (m)",
      "Pulkovo tmerc (m)",
      "1:500000",
      "1:200000",
      "1:100000",
      "1:50000",
  };

    for (int k=0; k<DLG_ROWS; k++){
      l[k] = Gtk::manage(new Gtk::Label(label_text[k], 1.0, 0.5));
      ex[k] = Gtk::manage (new Gtk::Entry);
      ey[k] = Gtk::manage (new Gtk::Entry);
      table.attach(*l[k],  0, 1, k, k+1); // left, right, top, buttom
      table.attach(*ex[k], 1, 2, k, k+1);
      if (k<5) table.attach(*ey[k], 2, 3, k, k+1);
      ex[k]->set_editable(false);
      ey[k]->set_editable(false);
    }
    get_vbox()->add(table);
    add_button (Gtk::Stock::CLOSE, Gtk::RESPONSE_OK);
    resize(1,1);
  }
  void on_response(int response) {std::cerr << response << "\n"; hide();}

  void set_crd(const int k, const dPoint & p){
    ex[k]->set_text(boost::lexical_cast<std::string>(p.x));
    ey[k]->set_text(boost::lexical_cast<std::string>(p.y));
  }
  void set_txt(const int k, const std::string & s){
    ex[k]->set_text(s);
  }
};

class ShowPt : public ActionMode {
public:
    ShowPt (Mapview * mapview) : ActionMode(mapview) { }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Show point information";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {dlg.hide();}

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) {

      if (!mapview->have_reference){
        mapview->statusbar.push("No geo-referenced layer.", 0);
        return;
      }
      convs::map2pt cnv1(mapview->reference, Datum("wgs84"), Proj("lonlat"));
      convs::map2pt cnv2(mapview->reference, Datum("pulk"),  Proj("lonlat"));
      dPoint p_wl=p; cnv1.frw(p_wl);
      dPoint p_pl=p; cnv2.frw(p_pl);
      Options O;
      O.put("lon0", convs::lon2lon0(p_wl.x));
      convs::map2pt cnv3(mapview->reference, Datum("wgs84"), Proj("tmerc"), O);
      O.put("lon0", convs::lon2lon0(p_pl.x));
      convs::map2pt cnv4(mapview->reference, Datum("pulk"),  Proj("tmerc"), O);
      dPoint p_wt=p; cnv3.frw(p_wt); p_wt.x+= convs::lon2pref(p_wl.x)*1e6;
      dPoint p_pt=p; cnv4.frw(p_pt); p_pt.x+= convs::lon2pref(p_pl.x)*1e6;

      dlg.set_crd(0,p);
      dlg.set_crd(1,p_wl);
      dlg.set_crd(2,p_pl);
      dlg.set_crd(3,p_wt);
      dlg.set_crd(4,p_pt);
      dlg.set_txt(5, convs::pt_to_nom(p_pl,500000));
      dlg.set_txt(6, convs::pt_to_nom(p_pl,200000));
      dlg.set_txt(7, convs::pt_to_nom(p_pl,100000));
      dlg.set_txt(8, convs::pt_to_nom(p_pl,50000));

      mapview->rubber.clear();
      mapview->rubber.add_src_mark(p);
      dlg.show_all();
    }

private:
    ShowPtDlg     dlg;
};

#endif /* AM_SHOW_PTH */
