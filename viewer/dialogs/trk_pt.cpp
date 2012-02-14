#include "trk_pt.h"
#include <sstream>
#include <iostream>
#include <iomanip>

DlgTrkPt::DlgTrkPt(){
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgTrkPt::hide_all)));

  // Labels
  Gtk::Label *l_alt  = manage(new Gtk::Label("Altitude:",      Gtk::ALIGN_RIGHT));
  Gtk::Label *l_time = manage(new Gtk::Label("Date and Time:", Gtk::ALIGN_RIGHT));
  l_alt->set_padding(3,0);
  l_time->set_padding(3,0);

  // Entries
  alt   = manage(new Gtk::Entry);
  time  = manage(new Gtk::Entry);
  coord = manage(new CoordBox);
  start = manage(new Gtk::CheckButton("Start point of a new segment"));

  // Table
  Gtk::Table *table = manage(new Gtk::Table(2,4));

            //  widget    l  r  t  b  x       y
  table->attach(*coord,   0, 2, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_alt,   0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*alt,     1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_time,  0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*time,    1, 2, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*start,   1, 2, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table);
}

void
DlgTrkPt::dlg2tpt(g_trackpoint * tpt) const{
  assert(tpt);
  tpt->dPoint::operator=(coord->get_ll());

  tpt->start = start->get_active();
  tpt->t = boost::lexical_cast<Time>(time->get_text());

  double d;
  std::istringstream s(alt->get_text());
  s >> d;
  if (s.fail()) tpt->clear_alt();
  else tpt->z=d;
}

void
DlgTrkPt::tpt2dlg(const g_trackpoint * tpt){
  assert(tpt);
  coord->set_ll(*tpt);
  start->set_active(tpt->start);
  time->set_text(boost::lexical_cast<std::string>(tpt->t));

  if (tpt->have_alt()){
    std::ostringstream s;
    s.setf(std::ios::fixed);
    s << std::setprecision(1) <<tpt->z;
    alt->set_text(s.str());
  }
  else alt->set_text("");
}

sigc::signal<void, dPoint>
DlgTrkPt::signal_jump(){
  return coord->signal_jump();
}
