#include "geo_data.h"

g_waypoint::g_waypoint (){
    x          = 0.0; 
    y          = 0.0; 
    z          = 1e24; 
    prox_dist  = 0.0;
    t          = Time(0);
    symb       = wpt_symb_enum.def; 
    displ      = 0; 
    color      = Color(0xFF000000); 
    bgcolor    = Color(0xFFFFFFFF); 
    map_displ  = wpt_map_displ_enum.def;
    pt_dir     = wpt_pt_dir_enum.def; 
    font_size  = 6;
    font_style = 0; 
    size       = 17;
    name       = "";
    comm       = "";
}

Options g_waypoint::to_options () const{
    Options opt;
    opt.put("lon", x);
    opt.put("lat", y);
    opt.put("height", z);
    opt.put("prox_dist", prox_dist);
    opt.put("t", t);
    opt.put("symb", symb);
    opt.put("displ", displ);
    opt.put("color", color);
    opt.put("bgcolor", bgcolor);
    opt.put("map_displ", map_displ);
    opt.put("pt_dir", pt_dir);
    opt.put("font_size", font_size);
    opt.put("font_style", font_style);
    opt.put("size", size);
    opt.put("name", name);
    opt.put("comm", comm);

    return opt;
}

void g_waypoint::parse_from_options (Options const & opt){
    g_waypoint p;
    swap(p);
    opt.get("lon", x);
    opt.get("lat", y);
    opt.get("height", z);
    opt.get("prox_dist", prox_dist);
    opt.get("t", t);
    opt.get("symb", symb);
    opt.get("displ", displ);
    opt.get("color", color);
    opt.get("bgcolor", bgcolor);
    opt.get("map_displ", map_displ);
    opt.get("pt_dir", pt_dir);
    opt.get("font_size", font_size);
    opt.get("font_style", font_style);
    opt.get("size", size);
    opt.get("name", name);
    opt.get("comm", comm);
}

g_trackpoint::g_trackpoint(){
    x   = 0; 
    y   = 0; 
    z   = 1e24; 
    depth = 1e24;
    start = false;
    t     = 0;
}

Options g_trackpoint::to_options() const{
    Options opt;
    opt.put("lon", x);
    opt.put("lat", y);
    opt.put("height", z);
    opt.put("depth", depth);
    opt.put("start", start);
    opt.put("t", t);
    return opt;
}

void g_trackpoint::parse_from_options(Options const & opt){
    g_trackpoint p;
    swap(p);
    opt.get("lon", x);
    opt.get("lat", y);
    opt.get("height", z);
    opt.get("depth", depth);
    opt.get("start", start);
    opt.get("t", t);
}

g_refpoint::g_refpoint(double _x, double _y, double _xr, double _yr){
    x=_x; y=_y;
    xr=_xr; yr=_yr;
}

g_refpoint::g_refpoint(g_point p, g_point r){
    x=p.x; y=p.y;
    xr=r.x; yr=r.y;
}

g_refpoint::g_refpoint(){
    x=0; y=0; xr=0; yr=0;
}

Options g_refpoint::to_options () const{
    Options opt;
    opt.put("lon", x);
    opt.put("lat", y);
    opt.put("xr", xr);
    opt.put("yr", yr);
    return opt;
}

void g_refpoint::parse_from_options (Options const & opt){
    g_refpoint p;
    swap(p);
    opt.get("lon", x);
    opt.get("lat", y);
    opt.get("xr", xr);
    opt.get("yr", yr);
}

/*********************************/
// lists
/*********************************/

g_waypoint_list::g_waypoint_list(){
    symbset = "garmin";
}

/// get range in lon-lat coords
g_rect g_waypoint_list::range() const{
    double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
    g_waypoint_list::const_iterator i;
    for (i=begin();i!=end();i++){
        if (i->x > maxx) maxx = i->x;
        if (i->y > maxy) maxy = i->y;
        if (i->x < minx) minx = i->x;
        if (i->y < miny) miny = i->y;
    }
    if ((minx>maxx)||(miny>maxy)) return g_rect(0,0,0,0);
    return g_rect(minx,miny, maxx-minx, maxy-miny);
}

/// convert waypoint_list values to Options object
Options g_waypoint_list::to_options () const {
    Options opt;
    opt.put("symbset", symbset);
    return opt;
}

/// set waypoint_list values from Options object
void g_waypoint_list::parse_from_options (Options const & opt){
    g_waypoint_list lst;
    swap(lst);
    opt.get("symbset", symbset);
}

g_track::g_track(){
    width = 2;
    displ = 1;
    color = Color(0xFF0000FF);
    skip  = 1;
    type  = trk_type_enum.def;
    fill  = trk_fill_enum.def;
    cfill = Color(0xFF000000);
    comm  = "";
}

Options g_track::to_options () const {
    Options opt;
    opt.put("width", width);
    opt.put("displ", displ);
    opt.put("color", color);
    opt.put("skip",  skip);
    opt.put("type",  type);
    opt.put("fill",  fill);
    opt.put("cfill", cfill);
    opt.put("comm",  comm);
    return opt;
}

void g_track::parse_from_options (Options const & opt){
    g_track lst;
    swap(lst);
    opt.get("width", width);
    opt.get("displ", displ);
    opt.get("color", color);
    opt.get("skip",  skip);
    opt.get("type",  type);
    opt.get("fill",  fill);
    opt.get("cfill", cfill);
    opt.get("comm",  comm);
}

/// get range in lon-lat coords
g_rect g_track::range() const{
    double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
    std::vector<g_trackpoint>::const_iterator i;
    for (i=begin();i!=end();i++){
        if (i->x > maxx) maxx = i->x;
        if (i->y > maxy) maxy = i->y;
        if (i->x < minx) minx = i->x;
        if (i->y < miny) miny = i->y;
    }
    if ((minx>maxx)||(miny>maxy)) return g_rect(0,0,0,0);
    return g_rect(minx,miny, maxx-minx, maxy-miny);
}


g_map & g_map::operator/= (double k){
    std::vector<g_refpoint>::iterator i;
    for (i=begin();i!=end();i++){
      i->xr /= k;
      i->yr /= k;
    }
    border/=k;
    return *this;
}

g_map & g_map::operator*= (double k){
  std::vector<g_refpoint>::iterator i;
  for (i=begin();i!=end();i++){
    i->xr *= k;
    i->yr *= k;
  }
  border*=k;
  return *this;
}

g_map & g_map::operator-= (g_point k){
  g_map::iterator i;
  for (i=begin();i!=end();i++){
      i->xr -= k.x;
      i->yr -= k.y;
  }
  border-=k;
  return *this;
}

g_map & g_map::operator+= (g_point k){
  g_map::iterator i;
  for (i=begin();i!=end();i++){
    i->xr += k.x;
    i->yr += k.y;
  }
  border+=k;
  return *this;
}


/// get range in lon-lat coords
/// диапазон карт определяется по точкам привязки, а не по
/// границам, поскольку здесь не очень хочется требовать наличия границ
/// и лазать в графический файлы карт за этими границами
g_rect g_map::range() const{
  double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
  g_map::const_iterator i;
  for (i=begin();i!=end();i++){
    if (i->x > maxx) maxx = i->x;
    if (i->y > maxy) maxy = i->y;
    if (i->x < minx) minx = i->x;
    if (i->y < miny) miny = i->y;
  }
  if ((minx>maxx)||(miny>maxy)) return g_rect(0,0,0,0);
  return g_rect(minx,miny, maxx-minx, maxy-miny);
}

/// clear all data
void geo_data::clear(){ 
  wpts.clear(); trks.clear(); maps.clear();
}

/// get range of all maps in lon-lat coords
/// диапазон карт определяется по точкам привязки, а не по
/// границам, поскольку здесь не очень хочется требовать наличия границ
/// и лазать в графический файлы карт за этими границами
g_rect geo_data::range_map() const{
  g_rect ret(0,0,0,0);
  if (maps.size()>0) ret=maps[0].range();
  else return ret;
  for (std::vector<g_map>::const_iterator i = maps.begin();
    i!=maps.end();i++) ret = rect_bounding_box(ret, i->range());
  return ret;
}

/// get range of all tracks and waypoints in lon-lat coords
g_rect geo_data::range_geodata() const{
  g_rect ret(0,0,0,0);
  if (wpts.size()>0) ret=wpts[0].range();
  else if (trks.size()>0) ret=trks[0].range();
  else return ret;

  for (std::vector<g_waypoint_list>::const_iterator i = wpts.begin(); 
    i!=wpts.end();i++) ret = rect_bounding_box(ret, i->range());
  for (std::vector<g_track>::const_iterator i = trks.begin(); 
    i!=trks.end();i++) ret = rect_bounding_box(ret, i->range());
  return ret;
}

/// get range of all data in lon-lat coords
g_rect geo_data::range() const{
  return rect_bounding_box(range_map(), range_geodata());
}

