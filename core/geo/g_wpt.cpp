#include "g_wpt.h"
#include "geo_convs.h"
#include "2d/line_utils.h"

#define ALT_UNDEF 1e27
#define ALT_CHECK 1e26

g_waypoint::g_waypoint (){
    x          = 0.0;
    y          = 0.0;
    z          = ALT_UNDEF;
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
    if (have_alt()) opt.put("alt", z);
    else opt.put<std::string>("alt", "");
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
    if ((opt.get<std::string>("alt") == "") ||
        (opt.get("alt", z)>ALT_CHECK)) clear_alt();
    else z = opt.get("alt", z);
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

    convs::pt2pt c(Datum(opt.get("datum", string("wgs84"))),
                   Proj(opt.get("proj", string("lonlat"))), opt,
                   Datum("wgs84"), Proj("lonlat"), opt);
    c.frw(*this);

    const std::string used[] = {
      "name", "comm", "lon", "lat", "alt", "prox_dist", "symb",
      "displ", "color", "bgcolor", "map_displ", "pt_dir", "font_size",
      "font_style", "size", "time", "x", "y",
      "datum", "proj", "lon0", "lat0", "E0", "N0", "k", ""
    };
    opt.warn_unused(used);
}

bool
g_waypoint::have_alt() const{
  return z<ALT_CHECK;
}
void
g_waypoint::clear_alt(){
  z=ALT_UNDEF;
}
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
    opt.put("comm",    comm);
    return opt;
}

/// set waypoint_list values from Options object
void g_waypoint_list::parse_from_options (Options const & opt){
    symbset = opt.get("symbset", symbset);
    comm    = opt.get("comm", comm);
    const std::string used[] = {
      "symbset","points","comm",""};
    opt.warn_unused(used);
}
