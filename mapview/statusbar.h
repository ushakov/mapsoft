#ifndef STATUSBAR_H
#define STATUSBAR_H

// StatusBar. Пока непонятно, чем отличается от обычного Gtk::Statusbar :)

class StatusBar : public Gtk::Statusbar{
  public:
  StatusBar(){
    push("Welcome to mapsoft viewer!",0);
  }
}

#endif
