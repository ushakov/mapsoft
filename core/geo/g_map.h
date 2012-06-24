#ifndef G_MAP_H
#define G_MAP_H

#include <vector>
#include <string>

#include "geo_types.h"
#include "2d/line.h"
#include "2d/rect.h"
#include "options/options.h"

/// reference point
struct g_refpoint : dPoint {
    double xr, yr; // raster points

    g_refpoint(double _x, double _y, double _xr, double _yr);
    g_refpoint(dPoint p, dPoint r);
    g_refpoint();
    Options to_options () const;
    void parse_from_options (Options const & opt);
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
    Options proj_opts;

    // these fildes used in tiled maps:
    int tsize;
    std::string tfmt;

    g_map();
    Options to_options () const;
    void parse_from_options (Options const & opt);

    g_map & operator/= (double k);
    g_map & operator*= (double k);
    g_map & operator-= (dPoint k);
    g_map & operator+= (dPoint k);

    /// get range of map (lon-lat) using refpoints and borders
    dRect range() const;
    dRect range_ref() const;   // lon-lat range of ref point
    dRect range_ref_r() const; // image coord range of ref point
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

/// map list
struct g_map_list : std::vector<g_map>{
    std::string comm;
    Options to_options () const;
    void parse_from_options (Options const & opt);
    dRect range() const;
    dRect range_correct() const;
};

#ifdef SWIG
%template(vector_g_refpoint) std::vector<g_refpoint>;
%template(vector_g_map)  std::vector<g_map>;
#endif  // SWIG


#endif
