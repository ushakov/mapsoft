#include "err.h"

DlgErr::DlgErr():
     Gtk::MessageDialog("", false,
                        Gtk::MESSAGE_WARNING, Gtk::BUTTONS_CLOSE){
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgErr::hide_all)));
}

void
DlgErr::call(const MapsoftErr & e){
  std::string msg = e.str() + "\n";
  if (get_visible()) msg = property_text().get_value() + msg;
  set_message(msg);
  show_all();
}
