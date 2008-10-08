#ifndef GEO_DATA_H
#define GEO_DATA_H

#include <vector>
#include <string>
#include <cmath>

#include "geo_names.h"
#include "geo_enums.h"
#include "../lib2d/line.h"
#include "../lib2d/rect.h"
#include "../utils/mapsoft_options.h"

#include "../utils/m_time.h"
#include "../utils/m_color.h"

/*********************************/
// points:

// abstract point
typedef Point<double> g_point;
typedef Line<double>  g_line;
typedef Rect<double>  g_rect;

/// single waypoint
struct g_waypoint : g_point {
    std::string  name;
    std::string  comm;
    Time         t;
    double  z;
    double  prox_dist;
    int     symb;
    int     displ;
    int     map_displ; 
    int     pt_dir;
    int     font_size;
    int     font_style;
    int     size;
    Color   color;
    Color   bgcolor;

    g_waypoint();
    Options to_options() const;
    void parse_from_options(Options const & opt);
};

/// single trackpoint
struct g_trackpoint : g_point {
    double z;
    double depth;
    bool start;
    Time t;

    g_trackpoint();
    Options to_options () const;
    void parse_from_options (Options const & opt);
};

/// reference point
struct g_refpoint : g_point {
    double xr, yr; // raster points

    g_refpoint(double _x, double _y, double _xr, double _yr);
    g_refpoint(g_point p, g_point r);
    g_refpoint();
    Options to_options () const;
    void parse_from_options (Options const & opt);
};

/*********************************/
// lists 
/*********************************/

/// g_waypoint_list : std::vecor<g_waypoint>
struct g_waypoint_list : std::vector<g_waypoint>{

    std::string symbset; /// garmin symbol set -- not used now

    g_waypoint_list();

    /// convert waypoint_list values to Options object
    Options to_options () const;

    /// set waypoint_list values from Options object
    void parse_from_options (Options const & opt);

    /// get range in lon-lat coords
    g_rect range() const;
};

/// track
struct g_track : std::vector<g_trackpoint>{
    int width; /// width of track plot line on screen (from OE)
    int displ; //  
    Color color; /// track color (RGB)
    int skip;  // 
    int type;
    int fill;  /// track fill style
    Color cfill; /// track fill color (RGB)
    std::string comm; /// track description

    g_track();
    Options to_options () const;
    void parse_from_options (Options const & opt);

    /// get range in lon-lat coords
    g_rect range() const;
};

/// map
struct g_map : std::vector<g_refpoint>,
  public boost::multiplicative<g_map,double>,
  public boost::additive<g_map, g_point>
{
    std::string comm;
    std::string file;
    Proj   map_proj;
    g_line border;

    g_map & operator/= (double k);
    g_map & operator*= (double k);
    g_map & operator-= (g_point k);
    g_map & operator+= (g_point k);

    /// get range in lon-lat coords
    g_rect range() const;
};

/*********************************/
// geo_data
/*********************************/

/// geo data
struct geo_data {
  std::vector<g_waypoint_list> wpts;
  std::vector<g_track> trks;
  std::vector<g_map> maps;

  /// clear all data
  void clear();

  /// get range of all maps in lon-lat coords
  g_rect range_map() const;
  /// get range of all tracks and waypoints in lon-lat coords
  g_rect range_geodata() const;
  /// get range of all data in lon-lat coords
  g_rect range() const;
};


#endif

