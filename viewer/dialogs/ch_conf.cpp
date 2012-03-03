#include "ch_conf.h"

DlgChConf::DlgChConf(){
  const char * text = "Some data may be lost. Continue?";
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgChConf::hide_all)));

  set_title(text);
  Gtk::Label *label  = manage(
    new Gtk::Label(text, Gtk::ALIGN_CENTER));
  label->set_padding(3,0);
  get_vbox()->add(*label);

  Gtk::Button * b = static_cast<Gtk::Button*>(
    get_widget_for_response(Gtk::RESPONSE_OK));
  b->signal_clicked().connect(
          sigc::mem_fun (this, &DlgChConf::on_ok));
}

void
DlgChConf::call(const sigc::slot<void> & slot){
  sigc::connection conn(current_slot);
  conn.disconnect();
  current_slot=slot;
  signal_ok_.connect(slot);
  show_all();
}

void
DlgChConf::on_ok(){
  signal_ok_.emit();
}
