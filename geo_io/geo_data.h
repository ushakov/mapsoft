#ifndef GEO_DATA_H
#define GEO_DATA_H

#include <vector>
#include <string>
#include "geo_enums.h"
#include "../utils/mapsoft_geo.h"
#include "../utils/point.h"


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

// waypoint list
struct g_waypoint_list
{
    std::string symbset;
    std::vector<g_waypoint> points;
    g_waypoint_list(){
	symbset = "garmin";
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

struct g_track {
    int width; // width of track plot line on screen (from OE)
    int displ; // 
    unsigned int color; // track color (RGB)
    int skip;  // 
    int type;
    int fill;  // track fill style
    unsigned int cfill; // track fill color (RGB)
    std::string comm; // track description
    std::vector<g_trackpoint> points;

    g_track()
	{
		width = 1;
		displ = 1;
		color = 0;
		skip  = 1;
		type  = trk_type_enum.def;
		fill  = trk_fill_enum.def;
		cfill = 0;
    }
};


struct g_refpoint : g_point {
	double xr, yr; // raster points
	g_refpoint(double _x = 0, double _y = 0, double _xr = 0, double _yr = 0){
		x=_x; y=_y;
		xr=_xr; yr=_yr;
	}
};


struct g_map {
	std::string comm;
	std::string file;
	std::vector<g_refpoint> points;
	std::vector<g_point>    border;
	Proj map_proj;
};


struct geo_data {
  std::vector<g_waypoint_list> wpts;
  std::vector<g_track> trks;
  std::vector<g_map> maps;
  void clear(){ wpts.clear(); trks.clear(); maps.clear();}
};

#endif

