#include "page_box.h"

double u2mm(int u, double d){
  switch (u){
    case 0: return 25.4/d; // px
    case 1: return 10.0;   // cm
    case 2: return 25.4;   // in
  }
}

double u2px(int u, double d){
  switch (u){
    case 1: return d/2.54; // cm
    case 2: return d;      // in
  }
}

double digs(int u){
  switch (u){
    case 0: return 0;      // px
    case 1: return 2;      // cm
    case 2: return 2;      // in
  }
}

PageBox::PageBox(): marg_adj(0, 0, 99),
                    x_adj(0,0,99999),
                    y_adj(0,0,99999),
                    dpi_adj(0, 0, 9999, 50) {

  /*** 1st hbox ***/
  Gtk::HBox * hb1 = manage(new Gtk::HBox);

  Gtk::Label *l1 = manage(
    new Gtk::Label("Size:",  Gtk::ALIGN_RIGHT));
  l1->set_padding(3,0);
  Gtk::Label *l2 = manage(
    new Gtk::Label("x",  Gtk::ALIGN_CENTER));
  l2->set_padding(3,0);
  Gtk::Label *l3 = manage(
    new Gtk::Label("DPI:",  Gtk::ALIGN_RIGHT));
  l3->set_padding(3,0);

  x   = manage(new Gtk::SpinButton(x_adj));
  y   = manage(new Gtk::SpinButton(y_adj));
  dpi = manage(new Gtk::SpinButton(dpi_adj));
  units = manage(new CBUnit());

  //                  expand, fill, padding
  hb1->pack_start(*l1, false, false, 3);
  hb1->pack_start(*x, false, false, 0);
  hb1->pack_start(*l2, false, false, 0);
  hb1->pack_start(*y, false, false, 0);
  hb1->pack_start(*units, false, false, 5);
  hb1->pack_start(*l3, false, false, 5);
  hb1->pack_start(*dpi, false, false, 3);

  /*** 2nd hbox ***/
  Gtk::HBox * hb2 = manage(new Gtk::HBox);

  Gtk::Label *l4 = manage(
    new Gtk::Label("Page:",  Gtk::ALIGN_RIGHT));
  l4->set_padding(3,0);
  Gtk::Label *l5 = manage(
    new Gtk::Label("Margins:",  Gtk::ALIGN_RIGHT));
  l5->set_padding(3,0);

  landsc = manage(new Gtk::CheckButton("Landscape"));
  marg = manage(new Gtk::SpinButton(marg_adj));
  page = manage(new CBPage());
  m_units = manage(new CBUnit());

  hb2->pack_start(*l4, false, false, 3);
  hb2->pack_start(*page, false, false, 3);
  hb2->pack_start(*l5, false, false, 3);
  hb2->pack_start(*marg, false, false, 3);
  hb2->pack_start(*m_units, false, false, 3);
  hb2->pack_start(*landsc, false, false, 3);

  /*** main vbox ***/
  Gtk::VBox * vb = manage(new Gtk::VBox);
  vb->pack_start(*hb1, false, false, 3);
  vb->pack_start(*hb2, false, false, 3);
  add(*vb);

  /*** default units ***/
  units->set_active_id(1);
  m_units->set_active_id(1);
  old_u = units->get_active_id();
  old_mu = m_units->get_active_id();
  x->set_digits(digs(old_u));
  y->set_digits(digs(old_u));
  marg->set_digits(digs(old_mu));
  marg->set_value(0.5);
  dpi->set_value(300.0);

  /*** signals ***/
  x->signal_changed().connect(
    sigc::mem_fun(this, &PageBox::ch_value));
  y->signal_changed().connect(
    sigc::mem_fun(this, &PageBox::ch_value));

  units->signal_changed().connect(
    sigc::mem_fun(this, &PageBox::ch_units));
  m_units->signal_changed().connect(
    sigc::mem_fun(this, &PageBox::ch_units));

  page->signal_changed().connect(
    sigc::mem_fun(this, &PageBox::ch_page));
  marg->signal_changed().connect(
    sigc::mem_fun(this, &PageBox::ch_page));
  dpi->signal_changed().connect(
    sigc::mem_fun(this, &PageBox::ch_page));
  landsc->signal_toggled().connect(
    sigc::mem_fun(this, &PageBox::ch_page));

  page->set_active(2); // set A4

}



void
PageBox::ch_units(){
  int mu = m_units->get_active_id();
  int u = units->get_active_id();
  double d = dpi->get_value();
  iPoint pi = page->get_active_id();
  if (u!=old_u){
    double f = u2mm(old_u,d) / u2mm(u,d);
    x->set_value( x->get_value() * f);
    y->set_value( y->get_value() * f);
    x->set_digits(digs(u));
    y->set_digits(digs(u));
  }
  if (mu!=old_mu){
    double f = u2mm(old_mu,d) / u2mm(mu,d);
    marg->set_value( marg->get_value() * f);
    marg->set_digits(digs(mu));
  }
  old_mu = mu;
  old_u = u;
  page->set_active_id(pi);
}

void
PageBox::ch_page(){
  iPoint pi = page->get_active_id();
  dPoint p(pi);
  if (p.x==0) { ch_units(); return; }

  double d = dpi->get_value();

  // convert margins to mm
  double m = marg->get_value() *
       u2mm(m_units->get_active_id(),d);
  p-=2*dPoint(m,m);
  if (p.x<0) p.x=0;
  if (p.y<0) p.y=0;

  // convert p to interface units:
  p/=u2mm(units->get_active_id(),d);

  bool sw = landsc->get_active();
  x->set_value(sw? p.y:p.x);
  y->set_value(sw? p.x:p.y);
  page->set_active_id(pi);
}

void
PageBox::ch_value(){
  page->set_active_id(iPoint(0,0));
}

dPoint
PageBox::get_px(){
  double f = u2px(units->get_active_id(), dpi->get_value());
  return dPoint(x->get_value(), y->get_value()) * f;
}

void
PageBox::set_px(const dPoint & p){
  double f = u2px(units->get_active_id(), dpi->get_value());
  x->set_value(p.x / f);
  y->set_value(p.y / f);
}

