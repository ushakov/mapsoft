#ifndef STATUSBAR_H
#define STATUSBAR_H

class StatusBar : Gtk::Statusbar{
  StatusBar(){
    status_bar->push("Welcome to mapsoft viewer!",0);
  }
}

#endif