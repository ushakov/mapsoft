#ifndef GEO_DATA_H
#define GEO_DATA_H

#include <vector>
#include <string>
#include <cmath>
#include "geo_names.h"
#include "geo_enums.h"
#include "../utils/point.h"
#include "../utils/rect.h"
#include "../utils/mapsoft_options.h"

/*********************************/
// points:

// abstract point
typedef Point<double> g_point;

// single waypoint
struct g_waypoint : g_point {
    double z;
    double prox_dist;
    time_t t;
    std::string name;
    std::string comm;
    int symb;
    int displ;
    unsigned int color;
    unsigned int bgcolor;
    int map_displ; 
    int pt_dir;
    int font_size;
    int font_style;
    int size;

    g_waypoint (){
		x=0.0; 
		y=0.0; 
		z=1e24; 
		prox_dist=0.0;
		t=0;
		symb=wpt_symb_enum.def; 
		displ=0; 
		color=0; 
		bgcolor    = 0xFFFFFF; 
		map_displ  = wpt_map_displ_enum.def;
		pt_dir     = wpt_pt_dir_enum.def; 
		font_size  = 6;
		font_style = 0; 
		size=17;
    }
};

// single trackpoint
struct g_trackpoint : g_point {
    double z;
    double depth;
    bool start;
    time_t t;
    g_trackpoint()
	{
    		x   = 0; 
		y   = 0; 
		z   = 1e24; 
		depth = 1e24;
    		start = false;
    		t     = 0;
	}
};

// reference point
struct g_refpoint : g_point {
	double xr, yr; // raster points
	g_refpoint(double _x = 0, double _y = 0, double _xr = 0, double _yr = 0){
		x=_x; y=_y;
		xr=_xr; yr=_yr;
	}
};

/*********************************/
// lists 
/*********************************/

// waypoint list
struct g_waypoint_list : std::vector<g_waypoint>{
    std::string symbset;
    g_waypoint_list(){
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
};

// track
struct g_track : std::vector<g_trackpoint>{
    int width; // width of track plot line on screen (from OE)
    int displ; // 
    unsigned int color; // track color (RGB)
    int skip;  // 
    int type;
    int fill;  // track fill style
    unsigned int cfill; // track fill color (RGB)
    std::string comm; // track description
    g_track(){
	width = 1;
	displ = 1;
	color = 0;
	skip  = 1;
	type  = trk_type_enum.def;
	fill  = trk_fill_enum.def;
	cfill = 0;
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
struct g_map : std::vector<g_refpoint>{
    std::string comm;
    std::string file;
    Proj map_proj;
    std::vector<g_point>    border;

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
    for (std::vector<g_map>::const_iterator i = maps.begin();
      i!=maps.end();i++) ret = rect_bounding_box(ret, i->range());
    return ret;
  }

  Rect<double> range_geodata() const{
    Rect<double> ret(0,0,0,0);
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

