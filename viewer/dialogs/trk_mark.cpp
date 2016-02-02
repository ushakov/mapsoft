#include "trk_mark.h"


DlgMarkTrk::DlgMarkTrk():  dist_adj(1.0,0.1,99.0), dist0_adj(0.0,0.0,99.0){
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgMarkTrk::hide_all)));

  rev  = manage(new Gtk::CheckButton("reverse direction"));
  dist = manage(new Gtk::SpinButton(dist_adj,0,1));
  Gtk::Label * dist_l = manage(new Gtk::Label("Distance between points, km:"));

  dist0 = manage(new Gtk::SpinButton(dist0_adj,0,1));
  Gtk::Label * dist0_l = manage(new Gtk::Label("Initial distance, km:"));

  // Table
  Gtk::Table *table = manage(new Gtk::Table(2,3));

            //  widget   l  r  t  b  x       y
  table->attach(*dist_l, 0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*dist,   1, 2, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*dist0_l, 0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*dist0,   1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*rev,    0, 2, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table);
}

bool
DlgMarkTrk::get_rev() const{
  return rev->get_active();
}

void
DlgMarkTrk::set_rev(const bool r){
  rev->set_active(r);
}

double
DlgMarkTrk::get_dist() const{
  return dist->get_value();
}

double
DlgMarkTrk::get_dist0() const{
  return dist0->get_value();
}
