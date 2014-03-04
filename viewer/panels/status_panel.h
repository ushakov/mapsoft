#ifndef VIEWER_STATUS_PANEL_H
#define VIEWER_STATUS_PANEL_H

// Panel with busy lamp and statusbar

#include <gtkmm.h>
#include <string>

class StatusPanel : public Gtk::HBox{
private:
  Gtk::Statusbar *statusbar;
  Gtk::Image *busy_icon;
public:
  // constructur -- no argumants
  StatusPanel(){
    busy_icon = manage(new Gtk::Image());
    statusbar = manage(new Gtk::Statusbar());
    busy_icon->set_tooltip_text("Viewer acivity");
    busy_icon->set_size_request(20,16);
    pack_start(*busy_icon, false, true, 0);
    pack_start(*statusbar, true, true, 0);
  }
  // put message on the statusbar
  void message(const std::string & msg){
    statusbar->push(msg,0);
  }
  // set busy icon
  void set_busy(){
    if (!is_realized()) return;
    busy_icon->set(Gtk::Stock::MEDIA_RECORD,Gtk::ICON_SIZE_MENU);
  }
  // unset busy icon
  void set_idle(){
    if (!is_realized()) return;
    busy_icon->clear();
  }
};

#endif
