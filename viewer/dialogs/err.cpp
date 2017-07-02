#include "err.h"

DlgErr::DlgErr():
     Gtk::MessageDialog("", false,
                        Gtk::MESSAGE_WARNING, Gtk::BUTTONS_CLOSE){
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgErr::hide_all)));
}

void
DlgErr::call(const Err & e){
  std::string msg = e.get_error() + "\n";
  if (get_visible()) msg = property_text().get_value() + msg;
  set_message(msg);
  show_all();
}
