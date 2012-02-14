#include "page_box.h"

PageBox::PageBox(): marg_adj(5, 0, 99),
                    x_adj(0,0,99999),
                    y_adj(0,0,99999),
                    dpi_adj(300, 0, 9999, 50) {

  /*** 1st hbox ***/
  Gtk::HBox * hb1 = manage(new Gtk::HBox);

  Gtk::Label *l1 = manage(
    new Gtk::Label("Size:",  Gtk::ALIGN_RIGHT));
  l1->set_padding(3,0);
  Gtk::Label *l2 = manage(
    new Gtk::Label("x",  Gtk::ALIGN_CENTER));
  l2->set_padding(3,0);
  Gtk::Label *l3 = manage(
    new Gtk::Label("DPI:",  Gtk::ALIGN_CENTER));
  l3->set_padding(3,0);

  x   = manage(new Gtk::SpinButton(x_adj));
  y   = manage(new Gtk::SpinButton(y_adj));
  dpi = manage(new Gtk::SpinButton(dpi_adj));
  units = manage(new SimpleCombo<int>);

  hb1->add(*l1);
  hb1->add(*x);
  hb1->add(*l2);
  hb1->add(*y);
  hb1->add(*units);
  hb1->add(*l3);
  hb1->add(*dpi);

  /*** 2nd hbox ***/
  Gtk::HBox * hb2 = manage(new Gtk::HBox);

  Gtk::Label *l4 = manage(
    new Gtk::Label("Page:",  Gtk::ALIGN_RIGHT));
  l4->set_padding(3,0);
  Gtk::Label *l5 = manage(
    new Gtk::Label("Margins [mm]:",  Gtk::ALIGN_RIGHT));
  l5->set_padding(3,0);

  landsc = manage(new Gtk::CheckButton("Landscape"));
  marg = manage(new Gtk::SpinButton(marg_adj));
  page = manage(new SimpleCombo<dPoint>);

  hb2->add(*l4);
  hb2->add(*page);
  hb2->add(*l5);
  hb2->add(*marg);
  hb2->add(*landsc);

  /*** main vbox ***/
  Gtk::VBox * vb = manage(new Gtk::VBox);
  vb->add(*hb1);
  vb->add(*hb2);
  add(*vb);
}

dPoint
PageBox::get_px(){
}

void
PageBox::set_px(const dPoint & p){
}

