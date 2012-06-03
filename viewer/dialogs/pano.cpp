#include "pano.h"


DlgPano::DlgPano(srtm3 * s): layer_pano(s){
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgPano::hide_all)));

  viewer = manage(new DThreadViewer(&layer_pano));

  simple_rainbow rb1(0,5000, RAINBOW_NORMAL);
  rb =  manage( new Rainbow(256, rb1.get_data(), rb1.get_size(),
    -999, 9999, 10, 0));

  get_vbox()->pack_start (*viewer, true, true);
  get_vbox()->pack_start (*rb, false, true);
  set_default_size(360,480);

  rb->signal_changed().connect(
      sigc::mem_fun(this, &DlgPano::on_ch));

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
  viewer->set_center(iPoint( width*angle/2.0/M_PI, viewer->get_center().y));
}
