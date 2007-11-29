#ifndef STATUSBAR_H
#define STATUSBAR_H

// StatusBar. Пока непонятно, чем отличается от обычного Gtk::Statusbar :)

class StatusBar : Gtk::Statusbar{
  StatusBar(){
    status_bar->push("Welcome to mapsoft viewer!",0);
  }
}

#endif
