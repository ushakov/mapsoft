#ifndef GEO_DATA_H
#define GEO_DATA_H

#include <vector>
#include <string>
#include <cmath>

#include <geo_io/geo_names.h>
#include <geo_io/geo_enums.h>
#include <utils/point.h>
#include <utils/line.h>
#include <utils/rect.h>
#include <utils/mapsoft_options.h>

#include <utils/generic_accessor.h>

#include <utils/m_time.h>
#include <utils/m_color.h>

/*********************************/
// points:

// abstract point
typedef Point<double> g_point;
typedef Line<double>  g_line;

// single waypoint
struct g_waypoint : g_point {
    double z;
    double prox_dist;
    Time t;
    std::string name;
    std::string comm;
    int symb;
    int displ;
    Color color;
    Color bgcolor;
    int map_displ; 
    int pt_dir;
    int font_size;
    int font_style;
    int size;

    g_waypoint (){
	set_default_values();
    }

    void set_default_values () {
	x=0.0; 
	y=0.0; 
	z=1e24; 
	prox_dist=0.0;
	t = Time(0);
	symb=wpt_symb_enum.def; 
	displ=0; 
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
    
    Options to_options () const{
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

    void parse_from_options (Options const & opt){
	set_default_values();
	
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
};

// single trackpoint
struct g_trackpoint : g_point {
    double z;
    double depth;
    bool start;
    Time t;
    g_trackpoint(){
	set_default_values();
    }

    void set_default_values()
    {
	x   = 0; 
	y   = 0; 
	z   = 1e24; 
	depth = 1e24;
	start = false;
	t     = 0;
    }
    
    Options to_options () const{
	Options opt;
	opt.put("lon", x);
	opt.put("lat", y);
	opt.put("height", z);
	opt.put("depth", depth);
	opt.put("start", start);
	opt.put("t", t);
	return opt;
    }

    void parse_from_options (Options const & opt){
	set_default_values();
	opt.get("lon", x);
	opt.get("lat", y);
	opt.get("height", z);
	opt.get("depth", depth);
	opt.get("start", start);
	opt.get("t", t);
    }

};

// reference point
struct g_refpoint : g_point {
    double xr, yr; // raster points
    g_refpoint(double _x, double _y, double _xr, double _yr){
	x=_x; y=_y;
    	xr=_xr; yr=_yr;
    }
    g_refpoint(){
	set_default_values();
    }
    void set_default_values(){
	x=0; y=0; xr=0; yr=0;
    }	
    Options to_options () const{
	Options opt;
	opt.put("lon", x);
	opt.put("lat", y);
	opt.put("xr", xr);
	opt.put("yr", yr);
	return opt;
    }
    void parse_from_options (Options const & opt){
	set_default_values();
	opt.get("lon", x);
	opt.get("lat", y);
	opt.get("xr", xr);
	opt.get("yr", yr);
    }

};

/*********************************/
// lists 
/*********************************/

// waypoint list
struct g_waypoint_list : std::vector<g_waypoint>{
    std::string symbset;
    g_waypoint_list(){
	set_default_values();
    }
    void set_default_values(){
	symbset = "garmin";
    }
    Rect<double> range() const{
      double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
      std::vector<g_waypoint>::const_iterator i;
      for (i=begin();i!=end();i++){
        if (i->x > maxx) maxx = i->x;
        if (i->y > maxy) maxy = i->y;
        if (i->x < minx) minx = i->x;
        if (i->y < miny) miny = i->y;
      }
      if ((minx>maxx)||(miny>maxy)) return Rect<double>(0,0,0,0);
      return Rect<double>(minx,miny, maxx-minx, maxy-miny);
    }
    Options to_options () const {
	Options opt;
	opt.put("symbset", symbset);
	return opt;
    }

    void parse_from_options (Options const & opt){
	set_default_values();
	opt.get("symbset", symbset);
    }

};

// track
struct g_track : std::vector<g_trackpoint>{
    int width; // width of track plot line on screen (from OE)
    int displ; // 
    Color color; // track color (RGB)
    int skip;  // 
    int type;
    int fill;  // track fill style
    Color cfill; // track fill color (RGB)
    std::string comm; // track description
    g_track(){
	set_default_values();
    }
    void set_default_values(){
	width = 2;
	displ = 1;
	color = Color(0xFF0000FF);
	skip  = 1;
	type  = trk_type_enum.def;
	fill  = trk_fill_enum.def;
	cfill = Color(0xFF000000);
	comm  = "";
    }

    Options to_options () const {
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
    void parse_from_options (Options const & opt){
	set_default_values();
	opt.get("width", width);
	opt.get("displ", displ);
	opt.get("color", color);
	opt.get("skip",  skip);
	opt.get("type",  type);
	opt.get("fill",  fill);
	opt.get("cfill", cfill);
	opt.get("comm",  comm);
    }

    Rect<double> range() const{
      double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
      std::vector<g_trackpoint>::const_iterator i;
      for (i=begin();i!=end();i++){
        if (i->x > maxx) maxx = i->x;
        if (i->y > maxy) maxy = i->y;
        if (i->x < minx) minx = i->x;
        if (i->y < miny) miny = i->y;
      }
      if ((minx>maxx)||(miny>maxy)) return Rect<double>(0,0,0,0);
      return Rect<double>(minx,miny, maxx-minx, maxy-miny);
    }
};

// map
struct g_map : 
  std::vector<g_refpoint>,     
  public boost::multiplicative<g_map,double>,
  public boost::additive<g_map, g_point>
{
    std::string comm;
    std::string file;
    Proj   map_proj;
    g_line border;

    g_map & operator/= (double k){
      std::vector<g_refpoint>::iterator i;
      for (i=begin();i!=end();i++){
          i->xr /= k;
          i->yr /= k;
      }
      return *this;
    }
    g_map & operator*= (double k){
      std::vector<g_refpoint>::iterator i;
      for (i=begin();i!=end();i++){
        i->xr *= k;
        i->yr *= k;
      }
      return *this;
    }
    g_map & operator-= (g_point k){
      std::vector<g_refpoint>::iterator i;
      for (i=begin();i!=end();i++){
          i->xr -= k.x;
          i->yr -= k.y;
      }
      return *this;
    }
    g_map & operator+= (g_point k){
      std::vector<g_refpoint>::iterator i;
      for (i=begin();i!=end();i++){
        i->xr += k.x;
        i->yr += k.y;
      }
      return *this;
    }



    Rect<double> range() const{
      double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);
      std::vector<g_refpoint>::const_iterator i;
      for (i=begin();i!=end();i++){
        if (i->x > maxx) maxx = i->x;
        if (i->y > maxy) maxy = i->y;
        if (i->x < minx) minx = i->x;
        if (i->y < miny) miny = i->y;
      }
      if ((minx>maxx)||(miny>maxy)) return Rect<double>(0,0,0,0);
      return Rect<double>(minx,miny, maxx-minx, maxy-miny);
    }
};

/*********************************/
// geo_data
/*********************************/

struct geo_data {
  std::vector<g_waypoint_list> wpts;
  std::vector<g_track> trks;
  std::vector<g_map> maps;

  void clear(){ wpts.clear(); trks.clear(); maps.clear();}

  Rect<double> range_map() const{ 
    // диапазон карт определяется по точкам привязки, а не по
    // границам, поскольку здесь не очень хочется требовать наличия границ
    // и лазать в графический файлы карт за этими границами
    Rect<double> ret(0,0,0,0);
    if (maps.size()>0) ret=maps[0].range();
    else return ret;
    for (std::vector<g_map>::const_iterator i = maps.begin();
      i!=maps.end();i++) ret = rect_bounding_box(ret, i->range());
    return ret;
  }

  Rect<double> range_geodata() const{
    Rect<double> ret(0,0,0,0);
    if (wpts.size()>0) ret=wpts[0].range();
    else if (trks.size()>0) ret=trks[0].range();
    else return ret;

    for (std::vector<g_waypoint_list>::const_iterator i = wpts.begin(); 
      i!=wpts.end();i++) ret = rect_bounding_box(ret, i->range());
    for (std::vector<g_track>::const_iterator i = trks.begin(); 
      i!=trks.end();i++) ret = rect_bounding_box(ret, i->range());
    return ret;
  }

  Rect<double> range() const{
    return rect_bounding_box(range_map(), range_geodata());
  }
};


#endif

