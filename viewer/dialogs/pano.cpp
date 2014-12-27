#include "pano.h"


DlgPano::DlgPano(srtm3 * s): gobj_pano(s),
                   dh_adj(20,0,9999,10), az_adj(0,-360,360,10),
                   viewer(&gobj_pano),
                   rubber(&viewer){
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgPano::hide_all)));

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

  get_vbox()->pack_start (viewer, true, true);
  get_vbox()->pack_start (*t, false, true);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  set_default_size(640,480);
  viewer.set_can_focus();
  viewer.grab_focus();

  rb->signal_changed().connect(
      sigc::mem_fun(this, &DlgPano::on_ch));
  dh->signal_value_changed().connect(
      sigc::mem_fun(this, &DlgPano::on_ch));
  az->signal_value_changed().connect(
      sigc::mem_fun(this, &DlgPano::set_az));

  viewer.signal_ch_origin().connect(
      sigc::mem_fun(this, &DlgPano::get_az));

  signal_key_press_event().connect (
    sigc::mem_fun (this, &DlgPano::on_key_press));

  // connect viewer click
  viewer.signal_click().connect(
    sigc::mem_fun (this, &DlgPano::click));

  gobj_pano.set_colors(rb->get_v1(), rb->get_v2());
  // viewer is not realized yet, we don't know its size..
  viewer.set_origin(gobj_pano.range().CNT()-iPoint(320,240));

}

void
DlgPano::on_ch(){
  gobj_pano.set_alt(dh->get_value());
  gobj_pano.set_colors( rb->get_v1(), rb->get_v2());
  viewer.redraw();
}

void
DlgPano::set_origin(const dPoint & pt){
  Gtk::Dialog::show_all();
  gobj_pano.set_origin(pt);
  viewer.redraw();
}

void
DlgPano::set_dir(const dPoint & pt){
  dPoint pt0 = gobj_pano.get_origin();
  double width = gobj_pano.get_width();
  double angle = atan2((pt.x-pt0.x)*cos(pt0.y*M_PI/180), pt.y-pt0.y);
  rubber.clear();
  viewer.set_center(iPoint( width*angle/2.0/M_PI, viewer.get_center().y));

  iPoint p = gobj_pano.geo2xy(pt);
  if (p.y>0) rubber.add_src_mark(p);
}

void
DlgPano::set_az(){
  if (!viewer.is_on_drag())
    viewer.set_center(iPoint(
      gobj_pano.get_width()/360.0*az->get_value(),
      viewer.get_center().y));
}

void
DlgPano::get_az(iPoint p){
  if (viewer.is_on_drag())
    az->set_value(
      (p.x + viewer.get_width()/2.0) * 360.0/gobj_pano.get_width());
}


bool
DlgPano::on_key_press(GdkEventKey * event) {
  switch (event->keyval) {
    case 43:
    case 61:
    case 65451: // + =
      viewer.rescale(2.0);
      return true;
    case 45:
    case 95:
    case 65453: // _ -
      viewer.rescale(0.5);
      return true;
  }
  return false;
}

void
DlgPano::click (iPoint p, int button, const Gdk::ModifierType & state) {
  dPoint pg=gobj_pano.xy2geo(p);
  rubber.clear();
  if (pg.y<90){
    rubber.add_src_mark(p);
    if (state&Gdk::CONTROL_MASK) signal_go_.emit(pg);
    else signal_point_.emit(pg);
  }
}

sigc::signal<void, dPoint>
DlgPano::signal_go(){
  return signal_go_;
}
sigc::signal<void, dPoint>
DlgPano::signal_point(){
  return signal_point_;
}
