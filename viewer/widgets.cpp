#include "widgets.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <geo/geo_nom.h>

CoordBox::CoordBox(){
  init();
}

CoordBox::CoordBox(BaseObjectType* cobject,
    const Glib::RefPtr<Gtk::Builder>& builder) :
      Gtk::Frame(cobject){
  init();
}

void
CoordBox::init(){
  set_label("Coordinates:");
  set_shadow_type(Gtk::SHADOW_ETCHED_IN);

  proj_cb.append("lonlat" /*, "Lon, Lat"*/);
  proj_cb.append("tmerc"  /*, "Gauss-Kruger"*/);
  proj_cb.set_active_text("lonlat");

  datum_cb.append("wgs84"   /*, "WGS84"*/);
  datum_cb.append("pulkovo" /*, "Pulkovo 1942"*/);
  datum_cb.set_active_text("wgs84");

  datum_cb.signal_changed().connect(
    sigc::mem_fun(this, &CoordBox::on_conv));
  proj_cb.signal_changed().connect(
    sigc::mem_fun(this, &CoordBox::on_conv));
  coords.signal_changed().connect(
    sigc::mem_fun(this, &CoordBox::on_change));

  Gtk::Button *jb = manage(new Gtk::Button);
  Gtk::IconSize isize=Gtk::ICON_SIZE_MENU;
  jb->set_relief(Gtk::RELIEF_NONE);
  jb->set_image(*manage(new Gtk::Image(Gtk::Stock::JUMP_TO, isize)));
  jb->signal_clicked().connect( sigc::mem_fun(this, &CoordBox::on_jump));

  // pack widgets
  Gtk::Table * table = manage(new Gtk::Table(5,2)); // table 4x2
  Gtk::Label * ldatum = manage(new Gtk::Label);
  Gtk::Label * lproj = manage(new Gtk::Label);
  ldatum->set_text("Datum:");
  lproj->set_text("Proj:");
            //  widget    l  r  t  b  x       y
  table->attach(coords,   0, 4, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*ldatum,  0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 3, 3);
  table->attach(datum_cb, 1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*lproj,   2, 3, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 3, 3);
  table->attach(proj_cb,  3, 4, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*jb,      4, 5, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
  add(*table);
}


void
CoordBox::set_ll(const dPoint &p){
  dPoint pc(p);
  Options O;
  O.put("lon0", convs::lon2lon0(p.x));
  convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(), datum, proj, O);
  cnv.frw(pc);
  int prec=6;
  if (proj == Proj("tmerc")){
    pc.x+= convs::lon2pref(p.x)*1e6;
    prec=0;
  }
  std::ostringstream ws;
  ws.setf(std::ios::fixed);
  ws << std::setprecision(prec) <<pc.x << ", " << pc.y;
  coords.set_text(ws.str());
  old_pt = p;
}

dPoint
CoordBox::get_xy(){
  dPoint ret;
  try {
    ret = boost::lexical_cast<dPoint>(coords.get_text());
  } catch (std::exception){
    set_ll(old_pt);
    return old_pt;
  }
  return ret;
}

dPoint
CoordBox::get_ll(){
  dPoint ret = get_xy();
  Options O;
  if (proj == Proj("tmerc")){
    O.put("lon0", convs::lon_pref2lon0(ret.x));
    ret.x = convs::lon_delprefix(ret.x);
  }
  convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
    datum, proj, O);
  cnv.bck(ret);
  return ret;
}

void
CoordBox::on_conv(){
  Datum new_datum(datum_cb.get_active_text());
  Proj new_proj(proj_cb.get_active_text());
  dPoint pt = get_xy();

  Options O;
  double lon0;
  if (proj == Proj("tmerc")){
    lon0 = convs::lon_pref2lon0(pt.x);
    pt.x = convs::lon_delprefix(pt.x);
  }
  else {
    lon0 = convs::lon2lon0(pt.x);
  }
  O.put("lon0", lon0);

  convs::pt2pt cnv(datum, proj, O, new_datum, new_proj, O);
  cnv.frw(pt);
  int prec=6;
  if (new_proj == Proj("tmerc")){
    pt.x+= convs::lon2pref(lon0)*1e6;
    prec=0;
  }
  std::ostringstream ws;
  ws.setf(std::ios::fixed);
  ws << std::setprecision(prec) <<pt.x << ", " << pt.y;
  coords.set_text(ws.str());
  datum=new_datum;
  proj=new_proj;}

void
CoordBox::on_change(){
  signal_changed_.emit();
}

void
CoordBox::on_jump(){
  signal_jump_.emit(get_ll());
}

sigc::signal<void> &
CoordBox::signal_changed(){
  return signal_changed_;
}

sigc::signal<void, dPoint> &
CoordBox::signal_jump(){
  return signal_jump_;
}


////

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

  const int rscale_num=4;
  std::pair<std::string, int> pa[rscale_num] = {
        std::pair<std::string,int>(" 1:50'000",  50000),
        std::pair<std::string,int>("1:100'000", 100000),
        std::pair<std::string,int>("1:200'000", 200000),
        std::pair<std::string,int>("1:500'000", 500000)
  };

  rscales.insert(&pa[0], &pa[rscale_num]);
  for (int i=0; i<rscale_num; i++) rscale.append(pa[i].first);
  rscale.set_active_text(pa[0].first);

  Gtk::Table * table = manage(new Gtk::Table(5,3));

  rscale.signal_changed().connect(
    sigc::mem_fun(this, &NomBox::on_change_rscale));

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

            //  widget    l  r  t  b  x       y
  table->attach(rscale,   0, 1, 0, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(nom,      1, 2, 0, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*bu[2],   2, 3, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
  table->attach(*bu[0],   3, 4, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
  table->attach(*bu[4],   3, 4, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
  table->attach(*bu[1],   3, 4, 2, 3, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
  table->attach(*bu[3],   4, 5, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
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
  std::map<std::string, int>::const_iterator mi;
  for (mi = rscales.begin(); mi!=rscales.end(); mi++){
    if (mi->second==rs){
      rscale.set_active_text(mi->first);
      break;
    }
  }

  if ((dx!=0) || (dy!=0))
    set_nom(convs::nom_shift(get_nom(), iPoint(dx,dy)));
  else
    signal_jump_.emit(pt);
}

int
NomBox::get_rscale(){
  std::map<std::string, int>::const_iterator mi =
    rscales.find(rscale.get_active_text());

  if (mi!=rscales.end()) return mi->second;
  else return 0;
}

sigc::signal<void, dPoint> &
NomBox::signal_jump(){
  return signal_jump_;
}
