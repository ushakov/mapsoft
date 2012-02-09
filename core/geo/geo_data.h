#ifndef GEO_DATA_H
#define GEO_DATA_H

#include <vector>
#include <string>
#include <cmath>

#include "geo_types.h"
#include "2d/line.h"
#include "2d/rect.h"
#include "options/options.h"

#include "options/m_time.h"
#include "options/m_color.h"

/*********************************/
// points:

/// single waypoint
struct g_waypoint : dPoint {
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

    bool have_alt() const;
    void clear_alt();
};

/// single trackpoint
struct g_trackpoint : dPoint {
    double z;
    double depth;
    bool start;
    Time t;

    g_trackpoint();
    Options to_options () const;
    void parse_from_options (Options const & opt);

    bool have_alt() const;
    void clear_alt();
    bool have_depth() const;
    void clear_depth();
};

/// reference point
struct g_refpoint : dPoint {
    double xr, yr; // raster points

    g_refpoint(double _x, double _y, double _xr, double _yr);
    g_refpoint(dPoint p, dPoint r);
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
    std::string comm;

    g_waypoint_list();

    /// convert waypoint_list values to Options object
    Options to_options () const;

    /// set waypoint_list values from Options object
    void parse_from_options (Options const & opt);

    /// get range in lon-lat coords
    dRect range() const;

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
    dRect range() const;
    /// get length in m
    double length() const;

    operator dLine() const;
};

/// map
struct g_map : std::vector<g_refpoint>
#ifndef SWIG
  , public boost::multiplicative<g_map,double>,
  public boost::additive<g_map, dPoint>
#endif  // SWIG
{
    std::string comm;
    std::string file;
    Proj   map_proj;
    dLine border;

    Options to_options () const;
    void parse_from_options (Options const & opt);

    g_map & operator/= (double k);
    g_map & operator*= (double k);
    g_map & operator-= (dPoint k);
    g_map & operator+= (dPoint k);

    /// get range of map (lon-lat) using refpoints and borders
    dRect range() const;
    dRect range_correct() const;
    /// get central point of map (lon-lat) using reference points
    dPoint center() const;

    // ensure the border is ok (uses file access and coordinate
    // conversion)
    void ensure_border();

#ifdef SWIG
    %extend {
      g_map operator/(double p) { return *$self / p; }
      g_map operator*(double p) { return *$self * p; }
      g_map operator-(const dPoint& p) { return *$self - p; }
      g_map operator+(const dPoint& p) { return *$self + p; }
    }
#endif  // SWIG
};

/// g_map_list : std::vecor<g_map>
struct g_map_list : std::vector<g_map>{
    std::string comm;
    Options to_options () const;
    void parse_from_options (Options const & opt);
    dRect range() const;
    dRect range_correct() const;
};

/*********************************/
// geo_data
/*********************************/

/// geo data
struct geo_data {
  std::vector<g_waypoint_list> wpts;
  std::vector<g_track> trks;
  std::vector<g_map_list> maps;

  /// clear all data
  void clear();

  /// get range of all maps in lon-lat coords, fast
  dRect range_map() const;
  /// get range of all maps in lon-lat coords using file size
  dRect range_map_correct() const;
  /// get range of all tracks and waypoints in lon-lat coords
  dRect range_geodata() const;
  /// get range of all data in lon-lat coords
  dRect range() const;

  // add data from another geo_data object
  void add(const geo_data & w);
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

