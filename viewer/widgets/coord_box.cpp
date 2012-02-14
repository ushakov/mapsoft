#include "coord_box.h"
#include <sstream>
#include <iomanip>

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
