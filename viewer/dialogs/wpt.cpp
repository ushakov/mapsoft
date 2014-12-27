#include "wpt.h"
#include <iomanip>
#include <sstream>

/********************************************************************/

DlgWpt::DlgWpt(): fs_adj(0,0,100), ps_adj(0,0,100){
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgWpt::hide_all)));

  // Labels
  Gtk::Label *l_name = manage(new Gtk::Label("Name:",        Gtk::ALIGN_RIGHT));
  Gtk::Label *l_comm = manage(new Gtk::Label("Comment:",     Gtk::ALIGN_RIGHT));
  Gtk::Label *l_alt  = manage(new Gtk::Label("Altitude:",     Gtk::ALIGN_RIGHT));
  Gtk::Label *l_time = manage(new Gtk::Label("Time:",        Gtk::ALIGN_RIGHT));
  Gtk::Label *l_fg   = manage(new Gtk::Label("Color:",       Gtk::ALIGN_RIGHT));
  Gtk::Label *l_bg   = manage(new Gtk::Label("Background:",  Gtk::ALIGN_RIGHT));
  Gtk::Label *l_fs   = manage(new Gtk::Label("Font size:",   Gtk::ALIGN_RIGHT));
  Gtk::Label *l_ps   = manage(new Gtk::Label("Point size:",  Gtk::ALIGN_RIGHT));
  l_name->set_padding(3,0);
  l_comm->set_padding(3,0);
  l_alt->set_padding(3,0);
  l_time->set_padding(3,0);
  l_fg->set_padding(3,0);
  l_bg->set_padding(3,0);
  l_fs->set_padding(3,0);
  l_ps->set_padding(3,0);

  // Entries
  fg   = manage(new Gtk::ColorButton);
  bg   = manage(new Gtk::ColorButton);
  name = manage(new Gtk::Entry);
  comm = manage(new Gtk::Entry);
  alt  = manage(new Gtk::Entry);
  time = manage(new Gtk::Entry);
  fs   = manage(new Gtk::SpinButton(fs_adj));
  ps   = manage(new Gtk::SpinButton(ps_adj));
  coord = manage(new CoordBox);

  // Table
  Gtk::Table *table = manage(new Gtk::Table(4,7));
            //  widget    l  r  t  b  x       y
  table->attach(*l_name,  0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*name,    1, 4, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_comm,  0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*comm,    1, 4, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_alt,   0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*alt,     1, 3, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_time,  0, 1, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*time,    1, 3, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_fg,    0, 1, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*fg,      1, 2, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_bg,    2, 3, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*bg,      3, 4, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_fs,    0, 1, 5, 6, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*fs,      1, 2, 5, 6, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_ps,    2, 3, 5, 6, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*ps,      3, 4, 5, 6, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*coord,   0, 4, 6, 7, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table);
}

void
DlgWpt::dlg2wpt(g_waypoint * wpt) const{
  assert(wpt);
  dPoint p = coord->get_ll();
  wpt->x=p.x; wpt->y=p.y;
  wpt->name = name->get_text();
  wpt->comm = comm->get_text();
  wpt->t    = boost::lexical_cast<Time>(time->get_text());
  wpt->font_size = (int)fs->get_value();
  wpt->size = (int)ps->get_value();
  Gdk::Color c = fg->get_color();
  wpt->color=
    (((unsigned)c.get_red()   & 0xFF00) >> 8) +
     ((unsigned)c.get_green() & 0xFF00) +
    (((unsigned)c.get_blue()  & 0xFF00) << 8);
  c = bg->get_color();
  wpt->bgcolor=
    (((unsigned)c.get_red()   & 0xFF00) >> 8) +
     ((unsigned)c.get_green() & 0xFF00) +
    (((unsigned)c.get_blue()  & 0xFF00) << 8);

  double d;
  std::istringstream s(alt->get_text());
  s >> d;
  if (s.fail()) wpt->clear_alt();
  else wpt->z=d;
}

void
DlgWpt::wpt2dlg(const g_waypoint * wpt){
  assert(wpt);
  coord->set_ll(*wpt);
  name->set_text(wpt->name);
  comm->set_text(wpt->comm);
  time->set_text(boost::lexical_cast<string>(wpt->t));
  fs->set_value(wpt->font_size);
  ps->set_value(wpt->size);
  Gdk::Color c;
  c.set_rgb((wpt->color & 0xFF)<<8,
            (wpt->color & 0xFF00),
            (wpt->color & 0xFF0000)>>8);
  fg->set_color(c);
  c.set_rgb((wpt->bgcolor & 0xFF)<<8,
            (wpt->bgcolor & 0xFF00),
            (wpt->bgcolor & 0xFF0000)>>8);
  bg->set_color(c);
  if (wpt->have_alt()){
    std::ostringstream s;
    s.setf(std::ios::fixed);
    s << std::setprecision(1) <<wpt->z;
    alt->set_text(s.str());
  }
  else{
    alt->set_text("");
  }
}

sigc::signal<void, dPoint>
DlgWpt::signal_jump(){
  return coord->signal_jump();
}
void
DlgWpt::set_ll(dPoint p){
  coord->set_ll(p);
}
