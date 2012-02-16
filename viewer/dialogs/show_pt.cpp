#include "show_pt.h"

DlgShowPt::DlgShowPt(){
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgShowPt::hide_all)));

  coord = manage(new CoordBox);
  nom = manage(new NomBox);

  get_vbox()->add(*coord);
  get_vbox()->add(*nom);

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
DlgShowPt::show_all(dPoint & p){
  Gtk::Dialog::show_all();
  coord->set_ll(p);
  nom->set_ll(p);
}
