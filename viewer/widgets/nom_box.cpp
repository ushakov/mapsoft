#include "nom_box.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <geo/geo_nom.h>

NomBox::NomBox() :
    cnv(Datum("wgs84"), Proj("lonlat"), Options(),
        Datum("pulk"), Proj("lonlat"), Options()) {
  init();
}

NomBox::NomBox(BaseObjectType* cobject,
    const Glib::RefPtr<Gtk::Builder>& builder) :
      Gtk::Frame(cobject),
    cnv(Datum("wgs84"), Proj("lonlat"), Options(),
        Datum("pulk"), Proj("lonlat"), Options()) {
  init();
}

void
NomBox::init(){
  set_label("Map name:");
  set_shadow_type(Gtk::SHADOW_ETCHED_IN);

  // Scale Combobox
  const int rscale_num=4;
  std::pair<int, std::string> pa[rscale_num] = {
        std::pair<int, std::string>( 50000, " 1:50'000"),
        std::pair<int, std::string>(100000, "1:100'000"),
        std::pair<int, std::string>(200000, "1:200'000"),
        std::pair<int, std::string>(500000, "1:500'000")
  };
  rscale.set_values(pa, pa + rscale_num);
  rscale.signal_changed().connect(
    sigc::mem_fun(this, &NomBox::on_change_rscale));

  // Buttons
  const int but_num=5;
  Gtk::Button *bu[but_num];
  Gtk::StockID bu_st[but_num] = {Gtk::Stock::GO_UP, Gtk::Stock::GO_DOWN,
     Gtk::Stock::GO_BACK, Gtk::Stock::GO_FORWARD, Gtk::Stock::JUMP_TO};
  int dx[but_num] = {0,0,-1,1,0};
  int dy[but_num] = {1,-1,0,0,0};
  Gtk::IconSize isize=Gtk::ICON_SIZE_MENU;

  for (int i=0; i<but_num; i++){
    bu[i] = manage(new Gtk::Button);
    bu[i]->set_relief(Gtk::RELIEF_NONE);
    bu[i]->set_image(*manage(new Gtk::Image(bu_st[i], isize)));
    bu[i]->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(this, &NomBox::move), dx[i],dy[i]));
  }
  bu[4]->set_tooltip_text("Jump to the map center");

  // Label
  Gtk::Label * lscale = manage(new Gtk::Label);
  lscale->set_text("Scale:");
  lscale->set_alignment(Gtk::ALIGN_RIGHT);
  lscale->set_padding(3,0);

  // Main table
  Gtk::Table * table = manage(new Gtk::Table(6,3));
            //  widget    l  r  t  b  x       y
  table->attach(nom,      0, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*lscale,  0, 1, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(rscale,   1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*bu[2],   2, 3, 0, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
  table->attach(*bu[0],   3, 4, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
  table->attach(*bu[1],   3, 4, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
  table->attach(*bu[3],   4, 5, 0, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
  table->attach(*bu[4],   5, 6, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
  add(*table);
}

void
NomBox::set_nom(const std::string &n){
  nom.set_text(n);
}

std::string
NomBox::get_nom() const{
  return nom.get_text();
}

void
NomBox::set_ll(const dPoint &p){
  pt=p;
  on_change_rscale();
}

dPoint
NomBox::get_ll() const{
  return pt;
}

void
NomBox::on_change_rscale(){
  dPoint pp=pt; cnv.frw(pp);
  set_nom(convs::pt_to_nom(pp, get_rscale()));
}

void
NomBox::move(int dx, int dy){
  // reset pt and rscale
  int rs;
  pt=convs::nom_to_range(nom.get_text(), rs).CNT();
  cnv.bck(pt);
  rscale.set_active_id(rs);

  if ((dx!=0) || (dy!=0))
    set_nom(convs::nom_shift(get_nom(), iPoint(dx,dy)));
  else
    signal_jump_.emit(pt);
}

int
NomBox::get_rscale(){
  return rscale.get_active_id();
}

sigc::signal<void, dPoint> &
NomBox::signal_jump(){
  return signal_jump_;
}
