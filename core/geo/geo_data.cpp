#include "geo_data.h"
#include "loaders/image_r.h"
#include "geo_convs.h"

g_waypoint::g_waypoint (){
    x          = 0.0;
    y          = 0.0;
    z          = 1e24;
    prox_dist  = 0.0;
    t          = Time(0);
    displ      = 0;
    color      = Color(0xFF000000); 
    bgcolor    = Color(0xFFFFFFFF); 
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
    opt.put("alt", z);
    opt.put("prox_dist", prox_dist);
    opt.put("time", t);
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
    x = opt.get("x", x);
    x = opt.get("lon", x);
    y = opt.get("y", y);
    y = opt.get("lat", y);
    z = opt.get("alt", z);
    prox_dist = opt.get("prox_dist", prox_dist);
    t = opt.get("time", t);
    symb = opt.get("symb", symb);
    displ = opt.get("displ", displ);
    color = opt.get("color", color);
    bgcolor = opt.get("bgcolor", bgcolor);
    map_displ = opt.get("map_displ", map_displ);
    pt_dir = opt.get("pt_dir", pt_dir);
    font_size = opt.get("font_size", font_size);
    font_style = opt.get("font_style", font_style);
    size = opt.get("size", size);
    name = opt.get("name", name);
    comm = opt.get("comm", comm);
    const std::string used[] = {
      "name", "comm", "lon", "lat", "alt", "prox_dist", "symb",
      "displ", "color", "bgcolor", "map_displ", "pt_dir", "font_size",
      "font_style", "size", "time", "x", "y", ""};
    opt.warn_unused(used);
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
    opt.put("alt", z);
    opt.put("depth", depth);
    opt.put("start", start);
    opt.put("time", t);
    return opt;
}

void g_trackpoint::parse_from_options(Options const & opt){
    g_trackpoint p;
    swap(p);
    x = opt.get("x", x);
    x = opt.get("lon", x);
    y = opt.get("y", y);
    y = opt.get("lat", y);
    z = opt.get("alt", z);
    depth = opt.get("depth", depth);
    start = opt.get("start", start);
    t = opt.get("time", t);
    const std::string used[] = {
      "lon", "lat", "alt", "depth", "start", "time", "x", "y", ""
    };
    opt.warn_unused(used);
}

g_refpoint::g_refpoint(double _x, double _y, double _xr, double _yr){
    x=_x; y=_y;
    xr=_xr; yr=_yr;
}

g_refpoint::g_refpoint(dPoint p, dPoint r){
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
    x  = opt.get("x",   x);
    x  = opt.get("lon", x);
    y  = opt.get("y",   y);
    y  = opt.get("lat", y);
    xr = opt.get("xr", xr);
    yr = opt.get("yr", yr);
    const std::string used[] = {
      "lon", "lat", "xr", "yr", "x", "y", ""
    };
    opt.warn_unused(used);
}

/*********************************/
// lists
/*********************************/

g_waypoint_list::g_waypoint_list(){
    symbset = "garmin";
}

/// get range in lon-lat coords
dRect g_waypoint_list::range() const{
    double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
    g_waypoint_list::const_iterator i;
    for (i=begin();i!=end();i++){
        if (i->x > maxx) maxx = i->x;
        if (i->y > maxy) maxy = i->y;
        if (i->x < minx) minx = i->x;
        if (i->y < miny) miny = i->y;
    }
    if ((minx>maxx)||(miny>maxy)) return dRect(0,0,0,0);
    return dRect(minx,miny, maxx-minx, maxy-miny);
}

/// convert waypoint_list values to Options object
Options g_waypoint_list::to_options () const {
    Options opt;
    opt.put("symbset", symbset);
    opt.put("points",  size());
    return opt;
}

/// set waypoint_list values from Options object
void g_waypoint_list::parse_from_options (Options const & opt){
    symbset = opt.get("symbset", symbset);
    const std::string used[] = {
      "symbset","points",""};
    opt.warn_unused(used);
}

g_track::g_track(){
    width = 2;
    displ = 1;
    color = Color(0xFF0000FF);
    skip  = 1;
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
    opt.put("points",  size());
    return opt;
}

void g_track::parse_from_options (Options const & opt){
    width = opt.get("width", width);
    displ = opt.get("displ", displ);
    color = opt.get("color", color);
    skip  = opt.get("skip",  skip);
    type  = opt.get("type",  type);
    fill  = opt.get("fill",  fill);
    cfill = opt.get("cfill", cfill);
    comm  = opt.get("comm",  comm);
    const std::string used[] = {
        "comm", "width", "color", "skip", "displ",
        "type", "fill", "cfill", "points", ""};
    opt.warn_unused(used);
}

/// get range in lon-lat coords
dRect g_track::range() const{
    double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
    std::vector<g_trackpoint>::const_iterator i;
    for (i=begin();i!=end();i++){
        if (i->x > maxx) maxx = i->x;
        if (i->y > maxy) maxy = i->y;
        if (i->x < minx) minx = i->x;
        if (i->y < miny) miny = i->y;
    }
    if ((minx>maxx)||(miny>maxy)) return dRect(0,0,0,0);
    return dRect(minx,miny, maxx-minx, maxy-miny);
}

double g_track::length() const{
  double ret=0;
  for (int i=0; i<(int)size()-1; i++){
    dPoint p1=(*this)[i] * M_PI/180.0;
    dPoint p2=(*this)[i+1] * M_PI/180.0;
    double cos_l=cos(p2.x-p1.x);
    double cos_f1=cos(p1.y);
    double sin_f1=sin(p1.y);
    double cos_f2=cos(p2.y);
    double sin_f2=sin(p2.y);
    double dd=acos(sin_f1*sin_f2 + cos_f1*cos_f2*cos_l);
    ret+=dd;
  }
  return ret*6380000.0;
}

g_track::operator dLine(void) const{
  dLine ret;
  for (g_track::const_iterator i=begin(); i!=end(); i++) ret.push_back(dPoint(*i));
  return ret;
}


Options g_map::to_options () const {
    Options opt;
    opt.put("comm",     comm);
    opt.put("file",     file);
    opt.put("map_proj", map_proj);
    opt.put("border",   border);
    opt.put("points",   size());
    return opt;
}

void g_map::parse_from_options (Options const & opt){
    comm     = opt.get("comm",     comm);
    file     = opt.get("file",     file);
    map_proj = opt.get("map_proj", map_proj);
    border   = opt.get("border",   border);

    // Add value of the "prefix" option to image path
    // (non-absolute paths only)
    std::string prefix=opt.get("prefix", std::string());
    if ((file.size()<1)||(file[0]!='/'))
        file=prefix+file;

    const std::string used[] = {
      "comm", "file", "map_proj", "border", "points", "prefix", ""};
    opt.warn_unused(used);
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

g_map & g_map::operator-= (dPoint k){
  g_map::iterator i;
  for (i=begin();i!=end();i++){
      i->xr -= k.x;
      i->yr -= k.y;
  }
  border-=k;
  return *this;
}

g_map & g_map::operator+= (dPoint k){
  g_map::iterator i;
  for (i=begin();i!=end();i++){
    i->xr += k.x;
    i->yr += k.y;
  }
  border+=k;
  return *this;
}

// Try to ensure the map has a border. If there is none, as the map
// file. If there is no map file (this->file == ""), alas, the border
// will be empty.
void g_map::ensure_border() {
  if (border.size() != 0) return;
  if (file == "") return;
  iPoint size = image_r::size(file.c_str());
  border.push_back(iPoint(0,0));
  border.push_back(iPoint(0,size.y));
  border.push_back(iPoint(size.x,size.y));
  border.push_back(iPoint(size.x,0));
}

/// get range in lon-lat coords
/// диапазон карт определяется по точкам привязки, и по границам, если
/// они есть
dRect g_map::range() const {
  double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
  g_map::const_iterator i;
  for (i = begin(); i != end(); ++i){
    if (i->x > maxx) maxx = i->x;
    if (i->y > maxy) maxy = i->y;
    if (i->x < minx) minx = i->x;
    if (i->y < miny) miny = i->y;
  }
  dLine::const_iterator j;
  convs::map2pt conv(*this, Datum("WGS84"), Proj("lonlat"));
  for (j = border.begin(); j != border.end(); ++j){
    dPoint p(*j); conv.frw(p);
    if (p.x > maxx) maxx = p.x;
    if (p.y > maxy) maxy = p.y;
    if (p.x < minx) minx = p.x;
    if (p.y < miny) miny = p.y;
  }
  if ((minx > maxx) || (miny > maxy)) return dRect(0, 0, 0, 0);
  return dRect(minx, miny, maxx-minx, maxy-miny);
}

/// get central point of map (lon-lat) using reference points
dPoint g_map::center() const {
  double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
  g_map::const_iterator i;
  for (i = begin(); i != end(); ++i){
    if (i->x > maxx) maxx = i->x;
    if (i->y > maxy) maxy = i->y;
    if (i->x < minx) minx = i->x;
    if (i->y < miny) miny = i->y;
  }
  if ((minx > maxx) || (miny > maxy)) return dPoint(0, 0);
  return dPoint((maxx+minx)/2, (maxy+miny)/2);
}

/// create g_map with borders from 
/// (using geom, scale/rscale, dpi, factor, proj, datum options)
void g_map::create_from_options(const Options & opt){
  comm     = opt.get("comm",     comm);
  file     = opt.get("file",     file);
  map_proj = opt.get("map_proj", map_proj);

  dRect geom = opt.get<dRect>("geom");
  Proj  proj(opt.get("proj", std::string("tmerc")));
  Datum datum(opt.get("datum", std::string("pulkovo")));

  double scale  = opt.get("scale",  1e-5);
  double rscale = opt.get("rscale", 0.0);
  double dpi    = opt.get("dpi",    200.0);
  double factor = opt.get("factor", 1.0);
  if (rscale!=0) scale=1.0/rscale; // rscale is prefered
  double k = scale/2.54e-2*dpi*factor;

  if (geom.empty()){
    std::cerr << "geo_data::g_map::create_from_options: "
         << "Error: empty geometry, use --geom option\n";
    exit(1);
  }

  // Conversion to target coordiates
  convs::pt2pt c(Datum("wgs84"), Proj("lonlat"), Options(),
                 datum, proj, opt);
  dLine brd;
  brd.push_back(geom.BLC());
  brd.push_back(geom.BRC());
  brd.push_back(geom.TRC());
  brd.push_back(geom.TLC());
  for (dLine::const_iterator p=brd.begin(); p!=brd.end(); p++){
    dPoint pg=*p;
    dPoint pr=*p;
    c.bck(pg);
    pr-=geom.TLC();
    pr.y=geom.h-pr.y;
    pr*=k;
    push_back(g_refpoint(pg, pr));
    border.push_back(pr);
  }
}


/// clear all data
void geo_data::clear(){ 
  wpts.clear(); trks.clear(); maps.clear();
}

/// get range of all maps in lon-lat coords
/// диапазон карт определяется по точкам привязки, и по границам, если
/// они есть
dRect geo_data::range_map() const {
  dRect ret(0,0,0,0);
  if (maps.size()>0) ret=maps[0].range();
  else return ret;
  for (std::vector<g_map>::const_iterator i = maps.begin();
    i!=maps.end();i++) ret = rect_bounding_box(ret, i->range());
  return ret;
}

/// get range of all maps in lon-lat coords
/// то же самое, но сначала делается попытка узнать границы из
/// графического файла, если их нет
dRect geo_data::range_map_correct() {
  dRect ret(0,0,0,0);
  if (maps.size()>0) ret=maps[0].range();
  else return ret;
  for (std::vector<g_map>::iterator i = maps.begin();
       i != maps.end();
       ++i) {
    i->ensure_border();
    ret = rect_bounding_box(ret, i->range());
  }
  return ret;
}

/// get range of all tracks and waypoints in lon-lat coords
dRect geo_data::range_geodata() const{
  dRect ret(0,0,0,0);
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
dRect geo_data::range() const{
  return rect_bounding_box(range_map(), range_geodata());
}

