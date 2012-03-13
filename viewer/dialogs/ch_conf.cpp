#include "ch_conf.h"

const char * ch_conf_text = "Some data may be lost. Continue?";

DlgChConf::DlgChConf():
     Gtk::MessageDialog(ch_conf_text, false,
                        Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL){
  signal_response().connect(
      sigc::mem_fun(this, &DlgChConf::on_result));
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
DlgChConf::on_result(int r){
  if (r == Gtk::RESPONSE_OK) signal_ok_.emit();
  hide_all();
}
