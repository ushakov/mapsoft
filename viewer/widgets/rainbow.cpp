#include "rainbow.h"
#include "utils/image_gdk.h"
#include <sstream>
#include <iomanip>

using namespace std;

Rainbow::Rainbow( int width, const rainbow_data * data, int size,
      int l, int h, int s, int d):
        v1a(l,l,h,s), v2a(h,l,h,s), img(width, 8, 0x00FFFFFF){

  dg=d;
  set_size_request(width * size / (size-1));
  double v1=data[0].v;
  double v2=data[size-1].v;
  if (v2<=v1) return;
  if (width < 2) return;

  // we need tails around min and max values
  double v1x = v1-(v2-v1)/(size-1)/2.0;
  double v2x = v2+(v2-v1)/(size-1)/2.0;

  // rainbow image
  for (int i = 0; i<width; i++){
    double v = v1x + i * (v2x-v1x) / (width-1);
    int c = get_rainbow(v, data, size);
    for (int j = 3; j < 8; j++) img.set(i,j,0xFF000000 | c);
  }

  // ticks and labels
  Gtk::HBox * hbox = manage(new Gtk::HBox());
  for (int i = 0; i<size; i++){
    int x = (i*(width-1))/size + (width-1)/size/2;
    for (int j = 0; j < 8; j++) img.set(x,j,0xFF000000);
    Gtk::Label * l = manage(new Gtk::Label());
    hbox->pack_start(*l, true, false, 1);
    labels.push_back(l);
  }

  Gtk::Table *t = manage(new Gtk::Table(4,3));
  Gtk::Label * v1l = manage(new Gtk::Label("from:", Gtk::ALIGN_RIGHT));
  Gtk::Label * v2l = manage(new Gtk::Label("to:", Gtk::ALIGN_RIGHT));
  v1e = manage(new Gtk::SpinButton(v1a, 0,d));
  v2e = manage(new Gtk::SpinButton(v2a, 0,d));
  v1e->set_value(v1);
  v2e->set_value(v2);
  Gtk::Image * i = manage(new Gtk::Image(make_pixbuf_from_image(img)));

  t->attach(*v1l,      0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*v1e,      1, 2, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*v2l,      2, 3, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*v2e,      3, 4, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*hbox,     0, 4, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  t->attach(*i,        0, 4, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);

  add(*t);
  on_ch();
  v1e->signal_value_changed().connect(
      sigc::mem_fun(this, &Rainbow::on_ch));
  v2e->signal_value_changed().connect(
      sigc::mem_fun(this, &Rainbow::on_ch));
}

void
Rainbow::on_ch(){
  int n = labels.size();
  double v1 = v1e->get_value();
  double v2 = v2e->get_value();
  for (int i = 0; i<n; i++){
    ostringstream s;
    s << fixed << setprecision(dg) << v1 + (v2-v1)*i/(n-1);
    labels[i]->set_text(s.str());
  }
  signal_changed_.emit();
}

void
Rainbow::set(double v1, double v2){
  v1e->set_value(v1);
  v2e->set_value(v2);
}

double
Rainbow::get_v1() const{
  return v1e->get_value();
}

double
Rainbow::get_v2() const{
  return v2e->get_value();
}

sigc::signal<void> &
Rainbow::signal_changed(){
  return signal_changed_;
}
