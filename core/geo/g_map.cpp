#include "g_map.h"
#include "loaders/image_r.h"
#include "geo_convs.h"
#include "2d/line_utils.h"
#include "2d/line_rectcrop.h"

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
  if (file == "") return;
  dRect file_range(dPoint(), image_r::size(file.c_str()));
  if (border.size() < 3 )
    border = rect2line(file_range);
  else
    rect_crop(file_range, border, true);
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

dRect g_map::range_correct() const {
  if (file == "") range();
  dRect file_range(dPoint(), image_r::size(file.c_str()));
  if (border.size() < 3 ) return file_range;
  dLine brd(border);
  rect_crop(file_range, brd, true);
  return brd.range();
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
  const std::string used[] = {"comm", ""};
  opt.warn_unused(used);
}

dRect
g_map_list::range() const{
  dRect ret(0,0,0,0);
  if (size()>0) ret=begin()->range();
  else return ret;

  for (std::vector<g_map>::const_iterator i = begin(); i!=end();i++)
    ret = rect_bounding_box(ret, i->range());
  return ret;
}

dRect
g_map_list::range_correct() const{
  dRect ret(0,0,0,0);
  if (size()>0) ret=begin()->range_correct();
  else return ret;

  for (std::vector<g_map>::const_iterator i = begin(); i!=end();i++)
    ret = rect_bounding_box(ret, i->range_correct());
  return ret;
}

