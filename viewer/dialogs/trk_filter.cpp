#include "trk_filter.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cassert>

DlgTrkFilter::DlgTrkFilter():  acc_adj(10,0,9999), num_adj(500,2,9999){
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgTrkFilter::hide_all)));

  // CheckButtons
  cb_acc  = manage(new Gtk::CheckButton("accuracy, m:"));
  cb_num  = manage(new Gtk::CheckButton("max number of points:"));
  cb_rg   = manage(new Gtk::CheckButton("remove gaps"));
  cb_acc->set_active();
  cb_num->set_active();
  cb_rg->set_active(false);

  // Entries
  acc   = manage(new Gtk::SpinButton(acc_adj));
  num   = manage(new Gtk::SpinButton(num_adj));

  cb_acc->signal_toggled().connect(
    sigc::mem_fun(this, &DlgTrkFilter::toggle_acc));
  cb_num->signal_toggled().connect(
    sigc::mem_fun(this, &DlgTrkFilter::toggle_num));

  Gtk::HSeparator * sep = manage(new Gtk::HSeparator());

  info  = manage(new Gtk::Label);

  // Table
  Gtk::Table *table = manage(new Gtk::Table(1,7));

            //  widget    l  r  t  b  x       y
  table->attach(*cb_acc,  0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*acc,     0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*sep,     0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*cb_num,  0, 1, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*num,     0, 1, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*info,    0, 1, 5, 6, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*cb_rg,   0, 1, 6, 7, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table);
}

double
DlgTrkFilter::get_acc() const{
  if (cb_acc->get_active()) return acc->get_value();
  else return 0.0;
}

int
DlgTrkFilter::get_num() const{
  if (cb_num->get_active()) return int(num->get_value());
  else return 0;
}

bool
DlgTrkFilter::get_rg() const{
  return cb_rg->get_active();
}

void
DlgTrkFilter::toggle_acc(){
  acc->set_sensitive(cb_acc->get_active());
}

void
DlgTrkFilter::toggle_num(){
  num->set_sensitive(cb_num->get_active());
}

void
DlgTrkFilter::set_info(const g_track * trk){
  assert(trk);
  std::ostringstream st;
  st << "Points: <b>"
     << trk->size() << "</b>, Length: <b>"
     << std::setprecision(2) << std::fixed
     << trk->length()/1000 << "</b> km";
        info->set_markup(st.str());
}
