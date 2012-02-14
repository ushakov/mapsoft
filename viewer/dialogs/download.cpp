#include "download.h"

DlgDownload::DlgDownload(){
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgDownload::hide_all)));

  cb_w   = manage(new Gtk::CheckButton("Waypoints"));
  cb_a   = manage(new Gtk::CheckButton("Active log"));
  cb_o   = manage(new Gtk::CheckButton("Saved tracks"));
  Gtk::VButtonBox * bbox =  manage(new Gtk::VButtonBox);
  bbox->add(*cb_w);
  bbox->add(*cb_a);
  bbox->add(*cb_o);
  Gtk::Frame * frame =
    manage(new Gtk::Frame("Select data to be downloaded:"));
  frame->add(*bbox);

  Gtk::Label * l_dev =
    manage(new Gtk::Label("GPS device:", Gtk::ALIGN_RIGHT));
  e_dev = manage(new Gtk::Entry);

  cb_off = manage(new Gtk::CheckButton("Turn off device after download"));

  Gtk::Table * table = manage(new Gtk::Table(2,3));
  table->attach(*frame,  0,2, 0,1, Gtk::FILL, Gtk::SHRINK, 3,3);
  table->attach(*l_dev,  0,1, 1,2, Gtk::SHRINK, Gtk::SHRINK, 3,3);
  table->attach(*e_dev,  1,2, 1,2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3,3);
  table->attach(*cb_off, 0,2, 2,3, Gtk::FILL, Gtk::SHRINK, 3,3);

  get_vbox()->add(*table);
}
