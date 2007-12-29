#ifndef VIEWER_AM_H
#define VIEWER_AM_H

#include <sys/time.h>
#include "../utils/point.h"
#include "../utils/log.h"

// ActionManager для DataList


class Mapview;

class ViewerAM{
  boost::shared_ptr<Viewer>  viewer;
  boost::shared_ptr<Mapview> mapview;
  Gtk::Menu *popup;

  public:
  ViewerAM(boost::shared_ptr<Viewer> viewer_, 
           boost::shared_ptr<Mapview> mapview_):
    viewer(viewer_), mapview(mapview_){

    // connect events from viewer
    viewer->signal_motion_notify_event().connect  (sigc::mem_fun (this, &ViewerAM::pointer_moved));
    viewer->signal_button_press_event().connect   (sigc::mem_fun (this, &ViewerAM::mouse_button_pressed));
    viewer->signal_button_release_event().connect (sigc::mem_fun (this, &ViewerAM::mouse_button_released));

    // creating popup menu
    mapview->uimanager->add_ui_from_string(
      "<ui>"
      "  <popup name='viewer_popup' action='viewer_popup'>"
      "    <menuitem name='quit'    action='quit'/>"
      "    <menuitem name='refresh' action='viewer_refresh'/>"
      "  </popup>"
      "</ui>"
    );
    mapview->actiongroup->add( Gtk::Action::create("viewer_refresh", "Refresh"), 
      sigc::mem_fun(*viewer, &Viewer::refresh));
    popup = dynamic_cast<Gtk::Menu*>(mapview->uimanager->get_widget("/viewer_popup"));

  }

  bool pointer_moved (GdkEventMotion * event) {
    Point<int> pos ((int) event->x, (int) event->y);
    VLOG(2) << "motion: " << pos << (event->is_hint? " hint ":"");
    if (!(event->state & Gdk::BUTTON1_MASK) || !event->is_hint) return false;
    Point<int> shift = pos - drag_pos;
    Point<int> window_origin = viewer->get_window_origin();
    window_origin -= shift;
    viewer->set_window_origin(window_origin);
    drag_pos = pos;
    // ask for more events
    viewer->get_pointer(pos.x, pos.y);
    return true;
  }

  bool mouse_button_pressed(GdkEventButton* event){
    VLOG(2) << "press: " << event->x << "," << event->y << " " << event->button;
    if (event->button == 1){
      drag_pos = Point<int> ((int)event->x, (int)event->y);
      gettimeofday (&click_started, NULL);
      return true;
    }
    if (event->button == 3){
      if(popup) popup->popup(event->button, event->time);
      return true;
    } 
    return false;
  }

  bool mouse_button_released (GdkEventButton * event) {
    VLOG(2) << "release: " << event->x << "," << event->y << " " << event->button;
    if (event->button == 1) {
      struct timeval click_ended;
      gettimeofday (&click_ended, NULL);
      int d = (click_ended.tv_sec - click_started.tv_sec) * 1000 + 
              (click_ended.tv_usec - click_started.tv_usec) / 1000; //ms
      if (d > 250) return false;
      Point<int> p(int(event->x), int(event->y));
      p += viewer->get_window_origin();
      VLOG(2) << "click at: " << p.x << "," << p.y << " " << event->button;
//      action_manager->click(p);
      return true;
    }
  }

  Point<int> drag_pos;
  struct timeval click_started;
};

#endif
