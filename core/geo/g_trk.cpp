#include "g_trk.h"
#include "geo_convs.h"
#include "2d/line_utils.h"

#define ALT_UNDEF 1e27
#define ALT_CHECK 1e26

g_trackpoint::g_trackpoint(){
    x   = 0; 
    y   = 0; 
    z   = ALT_UNDEF; 
    depth = ALT_UNDEF;
    start = false;
    t     = 0;
}

Options g_trackpoint::to_options() const{
    Options opt;
    opt.put("lon", x);
    opt.put("lat", y);
    if (have_alt()) opt.put("alt", z);
    else opt.put<std::string>("alt", "");
    if (have_depth()) opt.put("depth", depth);
    else opt.put<std::string>("depth", "");
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
    if ((opt.get<std::string>("alt") == "") ||
        (opt.get("alt", z)>ALT_CHECK)) clear_alt();
    else z = opt.get("alt", z);
    if ((opt.get<std::string>("depth") == "") ||
        (opt.get("depth", z)>ALT_CHECK)) clear_depth();
    else depth = opt.get("depth", z);
    depth = opt.get("depth", depth);
    start = opt.get("start", start);
    t = opt.get("time", t);
    const std::string used[] = {
      "lon", "lat", "alt", "depth", "start", "time", "x", "y", ""
    };
    opt.warn_unused(used);
}

bool
g_trackpoint::have_alt() const{
  return z<ALT_CHECK;
}
void
g_trackpoint::clear_alt(){
  z=ALT_UNDEF;
}
bool
g_trackpoint::have_depth() const{
  return depth<ALT_CHECK;
}
void
g_trackpoint::clear_depth(){
  depth=ALT_UNDEF;
}

/**************************/

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
