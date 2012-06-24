#include "g_map.h"
#include "loaders/image_r.h"
#include "geo_convs.h"
#include "2d/line_utils.h"
#include "2d/line_rectcrop.h"

using namespace std;

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
    convs::pt2pt c(Datum(opt.get("datum", string("wgs84"))),
                   Proj(opt.get("proj", string("lonlat"))), opt,
                   Datum("wgs84"), Proj("lonlat"), opt);
    c.frw(*this);

    const string used[] = {
      "lon", "lat", "xr", "yr", "x", "y",
      "datum", "proj", "lon0", "lat0", "E0", "N0", "k", ""
    };
    opt.warn_unused(used);
}

/*********************************/

g_map::g_map(){
  tsize=256;
  tfmt="%dx%d.jpg";
  tswap=false;
}

Options g_map::to_options () const {
    Options opt(proj_opts);
    opt.put("comm",      comm);
    opt.put("file",      file);
    opt.put("map_proj",  map_proj);
    opt.put("border",    border);
    opt.put("tile_size", tsize);
    opt.put("tile_fmt",  tfmt);
    if (tswap) opt.put("tile_swapy", "");
    opt.put("points",    size());

    return opt;
}

void g_map::parse_from_options (Options const & opt){
    comm     = opt.get("comm",     comm);
    file     = opt.get("file",     file);
    map_proj = opt.get("map_proj", map_proj);
    border   = opt.get("border",   border);
    tsize    = opt.get("tile_size", tsize);
    tfmt     = opt.get("tile_fmt",  tfmt);

    // Add value of the "prefix" option to image path
    // (non-absolute paths only)
    string prefix=opt.get("prefix", string());
    if ((file.size()<1)||(file[0]!='/'))
        file=prefix+file;

    if (opt.exists("lon0")) proj_opts.put("lon0", opt.get<string>("lon0"));
    if (opt.exists("lat0")) proj_opts.put("lat0", opt.get<string>("lat0"));
    if (opt.exists("E0")) proj_opts.put("E0", opt.get<string>("E0"));
    if (opt.exists("N0")) proj_opts.put("N0", opt.get<string>("N0"));
    if (opt.exists("tile_swapy")) tswap=true;

    const string used[] = {
      "comm", "file", "map_proj", "border", "points", "prefix",
      "tile_size", "tile_fmt", "tile_swapy", "lon0", "lat0" , "E0", "N0", ""};
    opt.warn_unused(used);
}

g_map & g_map::operator/= (double k){
    vector<g_refpoint>::iterator i;
    for (i=begin();i!=end();i++){
      i->xr /= k;
      i->yr /= k;
    }
    border/=k;
    return *this;
}

g_map & g_map::operator*= (double k){
  vector<g_refpoint>::iterator i;
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
  if (file == "") return;
  dRect file_range(dPoint(), image_r::size(file.c_str()));
  if (border.size() < 3 )
    border = rect2line(file_range);
  else
    rect_crop(file_range, border, true);
}

/// get range in lon-lat coords (border or refpoints)
dRect g_map::range() const {
  // try border
  if (border.size()>2){
    convs::map2pt cnv(*this, Datum("WGS84"), Proj("lonlat"));
    return cnv.line_frw(border).range();
  }
  // else use refpoints
  return range_ref();
}

dRect g_map::range_ref() const {
  if (size()<1) return dRect(0,0,0,0);
  double minx,maxx,miny,maxy;
  minx=maxx=(*this)[0].x;
  miny=maxy=(*this)[0].y;
  g_map::const_iterator i;
  for (i = begin(); i != end(); ++i){
    if (i->x > maxx) maxx = i->x;
    if (i->y > maxy) maxy = i->y;
    if (i->x < minx) minx = i->x;
    if (i->y < miny) miny = i->y;
  }
  return dRect(minx, miny, maxx-minx, maxy-miny);
}

dRect g_map::range_ref_r() const {
  if (size()<1) return dRect(0,0,0,0);
  double minx,maxx,miny,maxy;
  minx=maxx=(*this)[0].xr;
  miny=maxy=(*this)[0].yr;
  g_map::const_iterator i;
  for (i = begin(); i != end(); ++i){
    if (i->xr > maxx) maxx = i->xr;
    if (i->yr > maxy) maxy = i->yr;
    if (i->xr < minx) minx = i->xr;
    if (i->yr < miny) miny = i->yr;
  }
  return dRect(minx, miny, maxx-minx, maxy-miny);
}

dRect g_map::range_correct() const {
  if (file == "") range();
  dRect file_range(dPoint(), image_r::size(file.c_str()));
  if (border.size() < 3 ) return file_range;
  dLine brd(border);
  rect_crop(file_range, brd, true);
  return brd.range();
}

/// get central point of map (lon-lat) using border or reference points
dPoint g_map::center() const {
  return range().CNT();
}

/*********************************/

Options
g_map_list::to_options() const{
  Options opt;
  opt.put("comm", comm);
  return opt;
}

void
g_map_list::parse_from_options (Options const & opt){
  comm = opt.get("comm", comm);
  const string used[] = {"comm", ""};
  opt.warn_unused(used);
}

dRect
g_map_list::range() const{
  dRect ret(0,0,0,0);
  if (size()>0) ret=begin()->range();
  else return ret;

  for (vector<g_map>::const_iterator i = begin(); i!=end();i++)
    ret = rect_bounding_box(ret, i->range());
  return ret;
}

dRect
g_map_list::range_correct() const{
  dRect ret(0,0,0,0);
  if (size()>0) ret=begin()->range_correct();
  else return ret;

  for (vector<g_map>::const_iterator i = begin(); i!=end();i++)
    ret = rect_bounding_box(ret, i->range_correct());
  return ret;
}

