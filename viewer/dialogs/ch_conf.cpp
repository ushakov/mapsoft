#include "ch_conf.h"

const char * ch_conf_text = "Some data may be lost. Continue?";

DlgChConf::DlgChConf():
     Gtk::MessageDialog(ch_conf_text, false,
                        Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK_CANCEL){

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
