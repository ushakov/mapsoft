#include "draw_opt.h"

using namespace std;

Options
DlgDrawOpt::get_opt() const{
  Options o;
  if (m_normal->get_active()) o.put<string>("trk_draw_mode", "normal");
  if (m_speed->get_active())  o.put<string>("trk_draw_mode", "speed");
  if (m_height->get_active()) o.put<string>("trk_draw_mode", "height");
  if (dots->get_active())   o.put<bool>("trk_draw_dots", "");
  if (arrows->get_active()) o.put<bool>("trk_draw_arrows", "");
  o.put<int>("trk_draw_v1", rv->get_v1());
  o.put<int>("trk_draw_v2", rv->get_v2());
  o.put<int>("trk_draw_h1", rh->get_v1());
  o.put<int>("trk_draw_h2", rh->get_v2());
  return o;
}

void
DlgDrawOpt::set_opt(const Options & o){
  string mode = o.get<string>("trk_draw_mode", "normal");
  if (mode == "speed") m_speed->set_active();
  else if (mode == "m_height") m_height->set_active();
  else m_normal->set_active();

  if (o.exists("trk_draw_dots")) dots->set_active();
  if (o.exists("trk_draw_arrows")) arrows->set_active();
  rv->set(
    o.get<int>("trk_draw_v1", 0),
    o.get<int>("trk_draw_v2", 0)
  );
  rh->set(
    o.get<int>("trk_draw_h1", 0),
    o.get<int>("trk_draw_h2", 0)
  );
}

void
DlgDrawOpt::on_ch(int mode){
  // No need to emit signal if changes does not
  // affect the current mode.
  // We get mode for which signal must be emitted:
  // 0 - all modes, 1 - normal, 2 - speed, 3 - height
  if ( ((mode == 1) && !m_normal->get_active()) ||
       ((mode == 2) && !m_speed->get_active()) ||
       ((mode == 3) && !m_height->get_active()) ) return;
  signal_changed_.emit();
}

sigc::signal<void> &
DlgDrawOpt::signal_changed(){
  return signal_changed_;
}


DlgDrawOpt::DlgDrawOpt(){
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  Gtk::Frame *trk_frame = manage(new Gtk::Frame("Track options"));
  m_normal =  manage(new Gtk::RadioButton("Normal"));
  Gtk::RadioButtonGroup gr = m_normal->get_group();
  m_speed  =  manage(new Gtk::RadioButton(gr, "Speed"));
  m_height =  manage(new Gtk::RadioButton(gr, "Height"));
  m_normal->set_active();

  dots   = manage(new Gtk::CheckButton("draw dots"));
  arrows = manage(new Gtk::CheckButton("draw arrows"));

  simple_rainbow rb(0,1);
  rv =  manage(
    new Rainbow(256, rb.get_data(), rb.get_size(), 0, 999, 1, 1));
  rh =  manage(
    new Rainbow(256, rb.get_data(), rb.get_size(), -999, 9999, 10, 0));

  Gtk::Table *t = manage(new Gtk::Table(3,3));
  t->attach(*m_normal, 0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*dots,     1, 2, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*arrows,   2, 3, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*m_speed,  0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*rv,       1, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*m_height, 0, 1, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*rh,       1, 3, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  trk_frame->add(*t);
  get_vbox()->add(*trk_frame);

  m_normal->signal_toggled().connect(
      sigc::bind(sigc::mem_fun(this, &DlgDrawOpt::on_ch), 0));
  m_speed->signal_toggled().connect(
      sigc::bind(sigc::mem_fun(this, &DlgDrawOpt::on_ch), 0));
  m_height->signal_toggled().connect(
      sigc::bind(sigc::mem_fun(this, &DlgDrawOpt::on_ch), 0));

  dots->signal_toggled().connect(
      sigc::bind(sigc::mem_fun(this, &DlgDrawOpt::on_ch), 1));
  arrows->signal_toggled().connect(
      sigc::bind(sigc::mem_fun(this, &DlgDrawOpt::on_ch), 1));

  rv->signal_changed().connect(
      sigc::bind(sigc::mem_fun(this, &DlgDrawOpt::on_ch), 2));
  rh->signal_changed().connect(
      sigc::bind(sigc::mem_fun(this, &DlgDrawOpt::on_ch), 3));

}


