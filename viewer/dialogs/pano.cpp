#include "pano.h"


DlgPano::DlgPano(srtm3 * s): layer_pano(s),
                   dh_adj(20,0,9999,10), az_adj(0,-360,360,10){
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgPano::hide_all)));

  viewer = manage(new DThreadViewer(&layer_pano));

  simple_rainbow rb1(0,5000, RAINBOW_NORMAL);
  rb =  manage( new Rainbow(256, rb1.get_data(), rb1.get_size(),
    -999, 9999, 100, 0));

  az = manage(new Gtk::SpinButton(az_adj));
  dh = manage(new Gtk::SpinButton(dh_adj));
  Gtk::Label * azl = manage(new Gtk::Label("Azimuth:", Gtk::ALIGN_RIGHT));
  Gtk::Label * dhl = manage(new Gtk::Label("Altitude:", Gtk::ALIGN_RIGHT));

  Gtk::Table * t =   manage(new Gtk::Table(3,2));

          //  widget    l  r  t  b  x       y
  t->attach(*azl,   0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*az,    1, 2, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*dhl,   0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*dh,    1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*rb,    2, 3, 0, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->pack_start (*viewer, true, true);
  get_vbox()->pack_start (*t, false, true);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  set_default_size(640,480);
  viewer->set_can_focus();
  viewer->grab_focus();

  rb->signal_changed().connect(
      sigc::mem_fun(this, &DlgPano::on_ch));
  dh->signal_value_changed().connect(
      sigc::mem_fun(this, &DlgPano::on_ch));
  az->signal_value_changed().connect(
      sigc::mem_fun(this, &DlgPano::set_az));

  signal_key_press_event().connect (
    sigc::mem_fun (this, &DlgPano::on_key_press));
//  viewer->signal_button_press_event().connect (
//    sigc::mem_fun (this, &DlgPano::on_button_press));
//  viewer->signal_scroll_event().connect (
//    sigc::mem_fun (this, &DlgPano::on_scroll));

  layer_pano.set_colors(rb->get_v1(), rb->get_v2());
  viewer->set_center(layer_pano.range().CNT());
}

void
DlgPano::on_ch(){
  layer_pano.set_alt(dh->get_value());
  layer_pano.set_colors( rb->get_v1(), rb->get_v2());
  viewer->redraw();
}

void
DlgPano::set_origin(const dPoint & pt){
  Gtk::Dialog::show_all();
  layer_pano.set_origin(pt);
  viewer->redraw();
}

void
DlgPano::set_dir(const dPoint & pt){
  dPoint pt0 = layer_pano.get_origin();
  double width = layer_pano.get_width();
  double angle = atan2((pt.x-pt0.x)*cos(pt0.y*M_PI/180), pt.y-pt0.y);
  az->set_value(angle * 180/M_PI + 180);

  viewer->set_center(iPoint( width*angle/2.0/M_PI, viewer->get_center().y));
//  layer_pano.set_dest(pt);
//  viewer->redraw();
}

void
DlgPano::set_az(){
  double width = layer_pano.get_width();
  double angle = az->get_value() / 180.0*M_PI;
  viewer->set_center(iPoint(width/180.0*(az->get_value()-180), viewer->get_center().y));
}

bool
DlgPano::on_key_press(GdkEventKey * event) {
  switch (event->keyval) {
    case 43:
    case 61:
    case 65451: // + =
      viewer->rescale(2.0);
      return true;
    case 45:
    case 95:
    case 65453: // _ -
      viewer->rescale(0.5);
      return true;
  }
  return false;
}

//bool
//DlgPano::on_button_press(GdkEventButton * event) {
//  viewer->grab_focus();
//  return false;
//}

//bool
//DlgPano::on_scroll(GdkEventScroll * event) {
//  std::cerr << event->x << " " <<  event->x << " " << event->direction << "\n";
//
//  return false;
//}
