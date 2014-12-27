#include "trk.h"
#include <cassert>
#include <iomanip>
#include <sstream>

DlgTrk::DlgTrk(): width_adj(0,0,100){
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgTrk::hide_all)));

  // Labels
  Gtk::Label *l_comm  = manage(new Gtk::Label("Name:",       Gtk::ALIGN_RIGHT));
  Gtk::Label *l_width = manage(new Gtk::Label("Line Width:", Gtk::ALIGN_RIGHT));
  Gtk::Label *l_fg    = manage(new Gtk::Label("Color:",      Gtk::ALIGN_RIGHT));
  l_comm->set_padding(3,0);
  l_width->set_padding(3,0);
  l_fg->set_padding(3,0);

  // Entries
  fg    = manage(new Gtk::ColorButton);
  comm  = manage(new Gtk::Entry);
  width = manage(new Gtk::SpinButton(width_adj));
  info  = manage(new Gtk::Label);
  hint  = manage(new Gtk::Label);
  hint->set_line_wrap();

  // Table
  Gtk::Table *table = manage(new Gtk::Table(4,4));
            //  widget    l  r  t  b  x       y
  table->attach(*l_comm,  0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*comm,    1, 4, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_width, 0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*width,   1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_fg,    2, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*fg,      3, 4, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*info,    1, 4, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*hint,    0, 4, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table);
}


void
DlgTrk::dlg2trk(g_track * trk) const{
  assert(trk);
  trk->comm = comm->get_text();
  trk->width = (int)width->get_value();
  Gdk::Color c = fg->get_color();
  trk->color=
    (((unsigned)c.get_red()   & 0xFF00) >> 8) +
     ((unsigned)c.get_green() & 0xFF00) +
    (((unsigned)c.get_blue()  & 0xFF00) << 8);
}
void
DlgTrk::trk2dlg(const g_track * trk){
  assert(trk);
  comm->set_text(trk->comm);
  width->set_value(trk->width);
  Gdk::Color c;
  c.set_rgb((trk->color & 0xFF)<<8,
            (trk->color & 0xFF00),
            (trk->color & 0xFF0000)>>8);
  fg->set_color(c);
  set_info(trk);
}
void
DlgTrk::set_info(const g_track * trk){
  assert(trk);
  std::ostringstream st;
  st << "Points: <b>"
     << trk->size() << "</b>, Length: <b>"
     << std::setprecision(2) << std::fixed
     << trk->length()/1000 << "</b> km";
        info->set_markup(st.str());
}

void
DlgTrk::set_hint(const char * str){
  hint->set_markup(str);
}
