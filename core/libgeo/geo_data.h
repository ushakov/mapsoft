#ifndef GEO_DATA_H
#define GEO_DATA_H

#include <vector>
#include <string>
#include <cmath>

#include "geo_types.h"
#include "../lib2d/line.h"
#include "../lib2d/rect.h"
#include "../utils/options.h"

#include "../utils/m_time.h"
#include "../utils/m_color.h"

/*********************************/
// points:

// abstract point
typedef dPoint g_point;
typedef dLine  g_line;
typedef dRect  g_rect;

/// single waypoint
struct g_waypoint : g_point {
    std::string  name;
    std::string  comm;
    Time         t;
    double       z;
    double       prox_dist;
    wptSymb      symb;
    int          displ;
    wptMapDispl  map_displ;
    wptPtDir     pt_dir;
    int          font_size;
    int          font_style;
    int          size;
    Color        color;
    Color        bgcolor;

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

#ifdef SWIG
%template(vector_g_waypoint) std::vector<g_waypoint>;
%template(vector_g_trackpoint) std::vector<g_trackpoint>;
%template(vector_g_refpoint) std::vector<g_refpoint>;
#endif  // SWIG

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
    int     width; /// width of track plot line on screen (from OE)
    int     displ; //
    Color   color; /// track color (RGB)
    int     skip;  //
    trkType type;
    trkFill fill;  /// track fill style
    Color   cfill; /// track fill color (RGB)
    std::string comm; /// track description

    g_track();
    Options to_options () const;
    void parse_from_options (Options const & opt);

    /// get range in lon-lat coords
    g_rect range() const;
    /// get length in m
    double length() const;
};

/// map
struct g_map : std::vector<g_refpoint>
#ifndef SWIG
  , public boost::multiplicative<g_map,double>,
  public boost::additive<g_map, g_point>
#endif  // SWIG
{
    std::string comm;
    std::string file;
    Proj   map_proj;
    g_line border;

    Options to_options () const;
    void parse_from_options (Options const & opt);

    g_map & operator/= (double k);
    g_map & operator*= (double k);
    g_map & operator-= (g_point k);
    g_map & operator+= (g_point k);

    /// get range in lon-lat coords
    g_rect range() const;
    
    // ensure the border is ok (uses file access and coordinate
    // conversion)
    void ensure_border();

#ifdef SWIG
    %extend {
      g_map operator/(double p) { return *$self / p; }
      g_map operator*(double p) { return *$self * p; }
      g_map operator-(const g_point& p) { return *$self - p; }
      g_map operator+(const g_point& p) { return *$self + p; }
    }
#endif  // SWIG
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

  /// get range of all maps in lon-lat coords, fast
  g_rect range_map() const;
  /// get range of all maps in lon-lat coords, correct (try to compute
  /// borders)
  g_rect range_map_correct();
  /// get range of all tracks and waypoints in lon-lat coords
  g_rect range_geodata() const;
  /// get range of all data in lon-lat coords
  g_rect range() const;
};

template <typename T>
Options to_options_skipdef (const T & x){
    Options opt=x.to_options();
    T defx;
    Options def=defx.to_options();
    Options::iterator i,j;
    for (i=def.begin(); i!=def.end(); i++){
      j=opt.find(i->first);
      if ((j!=opt.end()) && (j->second==i->second)) opt.erase(j->first);
    }
    return opt;
}

#ifdef SWIG
%template(vector_g_waypoint_list)  std::vector<g_waypoint_list>;
%template(vector_g_track)  std::vector<g_track>;
%template(vector_g_map)  std::vector<g_map>;
#endif  // SWIG


#endif

