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

  // Comboboxes
  const int p_num=2;
  std::pair<Proj, std::string> p_list[p_num] = {
    std::pair<Proj, std::string>(Proj("lonlat"), "Lon, Lat"),
    std::pair<Proj, std::string>(Proj("tmerc"),  "Gauss-Kruger"),
  };
  const int d_num=2;
  std::pair<Datum, std::string> d_list[d_num] = {
    std::pair<Datum, std::string>(Datum("wgs84"), "WGS84"),
    std::pair<Datum, std::string>(Datum("pulk"),  "Pulkovo 1942"),
  };
  proj_cb.set_values(p_list, p_list+p_num);
  datum_cb.set_values(d_list, d_list+d_num);
  proj=proj_cb.get_active_id();
  datum=datum_cb.get_active_id();

  datum_cb.signal_changed().connect(
    sigc::mem_fun(this, &CoordBox::on_conv));
  proj_cb.signal_changed().connect(
    sigc::mem_fun(this, &CoordBox::on_conv));
  coords.signal_changed().connect(
    sigc::mem_fun(this, &CoordBox::on_change));

  // Jump button
  Gtk::Button *jb = manage(new Gtk::Button);
  Gtk::IconSize isize=Gtk::ICON_SIZE_MENU;
  jb->set_relief(Gtk::RELIEF_NONE);
  jb->set_image(*manage(new Gtk::Image(Gtk::Stock::JUMP_TO, isize)));
  jb->signal_clicked().connect( sigc::mem_fun(this, &CoordBox::on_jump));
  jb->set_tooltip_text("Jump to coordinates");

  // Labels
  Gtk::Label * ldatum = manage(new Gtk::Label);
  Gtk::Label * lproj = manage(new Gtk::Label);
  ldatum->set_text("Datum:");
  lproj->set_text("Proj:");
  ldatum->set_alignment(Gtk::ALIGN_RIGHT);
  lproj->set_alignment(Gtk::ALIGN_RIGHT);

  // Main table
  Gtk::Table * table = manage(new Gtk::Table(5,2));
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
  Datum new_datum(datum_cb.get_active_id());
  Proj new_proj(proj_cb.get_active_id());
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

/********************************************************************/

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

/********************************************************************/

DlgWpt::DlgWpt(): fs_adj(0,0,100), ps_adj(0,0,100){
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  // Labels
  Gtk::Label *l_name = manage(new Gtk::Label("Name:",        Gtk::ALIGN_RIGHT));
  Gtk::Label *l_comm = manage(new Gtk::Label("Comment:",     Gtk::ALIGN_RIGHT));
  Gtk::Label *l_alt  = manage(new Gtk::Label("Altitude:",     Gtk::ALIGN_RIGHT));
  Gtk::Label *l_fg   = manage(new Gtk::Label("Color:",       Gtk::ALIGN_RIGHT));
  Gtk::Label *l_bg   = manage(new Gtk::Label("Background:",  Gtk::ALIGN_RIGHT));
  Gtk::Label *l_fs   = manage(new Gtk::Label("Font size:",   Gtk::ALIGN_RIGHT));
  Gtk::Label *l_ps   = manage(new Gtk::Label("Point size:",  Gtk::ALIGN_RIGHT));
  l_name->set_padding(3,0);
  l_comm->set_padding(3,0);
  l_alt->set_padding(3,0);
  l_fg->set_padding(3,0);
  l_bg->set_padding(3,0);
  l_fs->set_padding(3,0);
  l_ps->set_padding(3,0);

  // Entries
  fg   = manage(new Gtk::ColorButton);
  bg   = manage(new Gtk::ColorButton);
  name = manage(new Gtk::Entry);
  comm = manage(new Gtk::Entry);
  alt  = manage(new Gtk::Entry);
  fs   = manage(new Gtk::SpinButton(fs_adj));
  ps   = manage(new Gtk::SpinButton(ps_adj));
  coord = manage(new CoordBox);

  // Table
  Gtk::Table *table = manage(new Gtk::Table(4,6));
            //  widget    l  r  t  b  x       y
  table->attach(*l_name,  0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*name,    1, 4, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_comm,  0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*comm,    1, 4, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_alt,   0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*alt,     1, 3, 2, 3, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_fg,    0, 1, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*fg,      1, 2, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_bg,    2, 3, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*bg,      3, 4, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_fs,    0, 1, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*fs,      1, 2, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_ps,    2, 3, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*ps,      3, 4, 4, 5, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*coord,   0, 4, 5, 6, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table);
}

void
DlgWpt::dlg2wpt(g_waypoint * wpt) const{
  assert(wpt);
  dPoint p = coord->get_ll();
  wpt->x=p.x; wpt->y=p.y;
  wpt->name = name->get_text();
  wpt->comm = comm->get_text();
  wpt->font_size = (int)fs->get_value();
  wpt->size = (int)ps->get_value();
  Gdk::Color c = fg->get_color();
  wpt->color.value=
    (((unsigned)c.get_red()   & 0xFF00) >> 8) +
     ((unsigned)c.get_green() & 0xFF00) +
    (((unsigned)c.get_blue()  & 0xFF00) << 8);
  c = bg->get_color();
  wpt->bgcolor.value=
    (((unsigned)c.get_red()   & 0xFF00) >> 8) +
     ((unsigned)c.get_green() & 0xFF00) +
    (((unsigned)c.get_blue()  & 0xFF00) << 8);

  double d;
  std::istringstream s(alt->get_text());
  s >> d;
  if (s.fail()) wpt->clear_alt();
  else wpt->z=d;
}

void
DlgWpt::wpt2dlg(const g_waypoint * wpt){
  assert(wpt);
  coord->set_ll(*wpt);
  name->set_text(wpt->name);
  comm->set_text(wpt->comm);
  fs->set_value(wpt->font_size);
  ps->set_value(wpt->size);
  Gdk::Color c;
  c.set_rgb((wpt->color.value & 0xFF)<<8,
            (wpt->color.value & 0xFF00),
            (wpt->color.value & 0xFF0000)>>8);
  fg->set_color(c);
  c.set_rgb((wpt->bgcolor.value & 0xFF)<<8,
            (wpt->bgcolor.value & 0xFF00),
            (wpt->bgcolor.value & 0xFF0000)>>8);
  bg->set_color(c);
  if (wpt->have_alt()){
    std::ostringstream s;
    s.setf(std::ios::fixed);
    s << std::setprecision(1) <<wpt->z;
    alt->set_text(s.str());
  }
  else{
    alt->set_text("");
  }
}

sigc::signal<void, dPoint>
DlgWpt::signal_jump(){
  return coord->signal_jump();
}
void
DlgWpt::set_ll(dPoint p){
  coord->set_ll(p);
}

/********************************************************************/

DlgTrk::DlgTrk(): width_adj(0,0,100){
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  // Labels
  Gtk::Label *l_comm  = manage(new Gtk::Label("Name:",       Gtk::ALIGN_RIGHT));
  Gtk::Label *l_width = manage(new Gtk::Label("Line Width:", Gtk::ALIGN_RIGHT));
  Gtk::Label *l_fg    = manage(new Gtk::Label("Color:",      Gtk::ALIGN_RIGHT));
  l_comm->set_padding(3,0);
  l_width->set_padding(3,0);
  l_fg->set_padding(3,0);

  // Entries
  fg    = manage(new Gtk::ColorButton);
  comm  = manage(new Gtk::Entry);
  width = manage(new Gtk::SpinButton(width_adj));
  info  = manage(new Gtk::Label);

  // Table
  Gtk::Table *table = manage(new Gtk::Table(4,3));
            //  widget    l  r  t  b  x       y
  table->attach(*l_comm,  0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*comm,    1, 4, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_width, 0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*width,   1, 2, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_fg,    2, 3, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*fg,      3, 4, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*info,    1, 4, 3, 4, Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table);
}


void
DlgTrk::dlg2trk(g_track * trk) const{
  assert(trk);
  trk->comm = comm->get_text();
  trk->width = (int)width->get_value();
  Gdk::Color c = fg->get_color();
  trk->color.value=
    (((unsigned)c.get_red()   & 0xFF00) >> 8) +
     ((unsigned)c.get_green() & 0xFF00) +
    (((unsigned)c.get_blue()  & 0xFF00) << 8);
}
void
DlgTrk::trk2dlg(const g_track * trk){
  assert(trk);
  comm->set_text(trk->comm);
  width->set_value(trk->width);
  Gdk::Color c;
  c.set_rgb((trk->color.value & 0xFF)<<8,
            (trk->color.value & 0xFF00),
            (trk->color.value & 0xFF0000)>>8);
  fg->set_color(c);
  set_info(trk);
}
void
DlgTrk::set_info(const g_track * trk){
  assert(trk);
  std::ostringstream st;
  st << "Points: <b>"
     << trk->size() << "</b>, Length: <b>"
     << std::setprecision(2) << std::fixed
     << trk->length()/1000 << "</b> km";
        info->set_markup(st.str());
}

/********************************************************************/

DlgShowPt::DlgShowPt(){
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  coord = manage(new CoordBox);
  nom = manage(new NomBox);

  get_vbox()->add(*coord);
  get_vbox()->add(*nom);

  coord->signal_jump().connect(
      sigc::mem_fun (this, &DlgShowPt::jump));
  nom->signal_jump().connect(
      sigc::mem_fun (this, &DlgShowPt::jump));
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgShowPt::hide_all)));
}

sigc::signal<void, dPoint> &
DlgShowPt::signal_jump(){
  return signal_jump_;
}

void
DlgShowPt::jump(const dPoint p){
  coord->set_ll(p);
  signal_jump_.emit(p);
}

void
DlgShowPt::show_all(dPoint & p){
  Gtk::Dialog::show_all();
  coord->set_ll(p);
  nom->set_ll(p);
}
