#include "pano.h"


DlgPano::DlgPano(srtm3 * s): layer_pano(s){
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgPano::hide_all)));

  viewer = manage(new DThreadViewer(&layer_pano));

  simple_rainbow rb1(0,5000, RAINBOW_NORMAL);
  rb =  manage( new Rainbow(256, rb1.get_data(), rb1.get_size(),
    -999, 9999, 10, 0));

  get_vbox()->pack_start (*viewer, true, true);
  get_vbox()->pack_start (*rb, false, true);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  set_default_size(360,480);

  rb->signal_changed().connect(
      sigc::mem_fun(this, &DlgPano::on_ch));
  signal_key_press_event().connect (
    sigc::mem_fun (this, &DlgPano::on_key_press));
//  viewer->signal_button_press_event().connect (
//    sigc::mem_fun (this, &DlgPano::on_button_press));
//  viewer->signal_scroll_event().connect (
//    sigc::mem_fun (this, &DlgPano::on_scroll));

  Options o = layer_pano.get_opt();
  o.put<double>("pano_alt", 20);
  o.put<double>("pano_minh", rb->get_v1());
  o.put<double>("pano_maxh", rb->get_v2());
  layer_pano.set_opt(o);
}

void
DlgPano::on_ch(){
  Options o = layer_pano.get_opt();
  o.put<double>("pano_alt", 20);
  o.put<double>("pano_minh", rb->get_v1());
  o.put<double>("pano_maxh", rb->get_v2());
  layer_pano.set_opt(o);
  viewer->redraw();
}

void
DlgPano::show_all(const dPoint & pt){
  Options o = layer_pano.get_opt();
  o.put("pano_pt", pt);
  Gtk::Dialog::show_all();
  layer_pano.set_opt(o);
  viewer->set_center(layer_pano.range().CNT());
  viewer->redraw();
}

void
DlgPano::set_dir(const dPoint & pt){
  Options o = layer_pano.get_opt();
  dPoint pt0 = o.get<dPoint>("pano_pt");
  double width = o.get<double>("pano_width");
  double angle = atan2((pt.x-pt0.x)*cos(pt0.y*M_PI/180), pt.y-pt0.y);
std::cerr << "AAA" << angle << "\n";
  viewer->set_center(iPoint( width*angle/2.0/M_PI, viewer->get_center().y));
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

