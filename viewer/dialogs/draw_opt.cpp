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
  o.put<int>("trk_draw_v1", v1->get_value());
  o.put<int>("trk_draw_v2", v2->get_value());
  o.put<int>("trk_draw_h1", h1->get_value());
  o.put<int>("trk_draw_h2", h2->get_value());
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
  v1->set_value(o.get<int>("trk_draw_v1", 0));
  v2->set_value(o.get<int>("trk_draw_v2", 10));
  h1->set_value(o.get<int>("trk_draw_v1", 0));
  h2->set_value(o.get<int>("trk_draw_v2", 1000));
}

void
DlgDrawOpt::on_ch(void){
  signal_change_.emit();
}

sigc::signal<void> &
DlgDrawOpt::signal_change(){
  return signal_change_;
}


DlgDrawOpt::DlgDrawOpt():
    v1a(0,0,999), v2a(10,0,999), h1a(0,-999,9999), h2a(1000,-999,9999){
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
  v1 = manage(new Gtk::SpinButton(v1a, 0,1));
  v2 = manage(new Gtk::SpinButton(v2a, 0,1));
  h1 = manage(new Gtk::SpinButton(h1a, 0,0));
  h2 = manage(new Gtk::SpinButton(h2a, 0,0));
  Gtk::Label * v1l = manage(new Gtk::Label("lower value:"));
  Gtk::Label * v2l = manage(new Gtk::Label("upper value:"));
  Gtk::Label * h1l = manage(new Gtk::Label("lower value:"));
  Gtk::Label * h2l = manage(new Gtk::Label("upper value:"));

  Gtk::Table *t = manage(new Gtk::Table(4,3));
  t->attach(*m_normal, 0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*dots,     1, 2, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*arrows,   2, 3, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*m_speed,  0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*v1l,      1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*v1,       2, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*v2l,      3, 4, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*v2,       4, 5, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*m_height, 0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*h1l,      1, 2, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*h1,       2, 3, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*h2l,      3, 4, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*h2,       4, 5, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  trk_frame->add(*t);
  get_vbox()->add(*trk_frame);

  m_normal->signal_toggled().connect(
      sigc::mem_fun(this, &DlgDrawOpt::on_ch));
  m_speed->signal_toggled().connect(
      sigc::mem_fun(this, &DlgDrawOpt::on_ch));
  m_height->signal_toggled().connect(
      sigc::mem_fun(this, &DlgDrawOpt::on_ch));

  v1->signal_value_changed().connect(
      sigc::mem_fun(this, &DlgDrawOpt::on_ch));
  v2->signal_value_changed().connect(
      sigc::mem_fun(this, &DlgDrawOpt::on_ch));
  h1->signal_value_changed().connect(
      sigc::mem_fun(this, &DlgDrawOpt::on_ch));
  h2->signal_value_changed().connect(
      sigc::mem_fun(this, &DlgDrawOpt::on_ch));

  dots->signal_toggled().connect(
      sigc::mem_fun(this, &DlgDrawOpt::on_ch));
  arrows->signal_toggled().connect(
      sigc::mem_fun(this, &DlgDrawOpt::on_ch));
}


