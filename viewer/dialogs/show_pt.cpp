#include "show_pt.h"
#include <sstream>
#include <iomanip>

using namespace std;

DlgShowPt::DlgShowPt(){
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgShowPt::hide_all)));

  coord = manage(new CoordBox);
  nom = manage(new NomBox);

  srtm_h = manage(new Gtk::Label("", Gtk::ALIGN_LEFT));
  srtm_h->set_use_markup();

  get_vbox()->add(*coord);
  get_vbox()->add(*nom);
  get_vbox()->add(*srtm_h);

  coord->signal_jump().connect(
      sigc::mem_fun (this, &DlgShowPt::jump));
  nom->signal_jump().connect(
      sigc::mem_fun (this, &DlgShowPt::jump));
}

sigc::signal<void, dPoint> &
DlgShowPt::signal_jump(){
  return signal_jump_;
}

void
DlgShowPt::jump(const dPoint p){
  coord->set_ll(p);
  signal_jump_.emit(p);
}

void
DlgShowPt::show_all(dPoint & p, double alt){
  Gtk::Dialog::show_all();
  coord->set_ll(p);
  nom->set_ll(p);
  ostringstream s;
  s << "SRTM altitude: <b>"
    << fixed << setprecision(1) << alt << "</b>";
  srtm_h->set_label(s.str());
}
