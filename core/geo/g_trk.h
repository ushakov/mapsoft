#ifndef G_TRK_H
#define G_TRK_H

#include <vector>
#include <string>

#include "geo_types.h"
#include "2d/line.h"
#include "2d/rect.h"
#include "options/options.h"

#include "options/m_time.h"
#include "options/m_color.h"

/// single trackpoint
struct g_trackpoint : dPoint {
    double z;
    double depth;
    bool start;
    Time t;

    g_trackpoint();
    Options to_options () const;
    void parse_from_options (Options const & opt, bool check=true);

    bool have_alt() const;
    void clear_alt();
    bool have_depth() const;
    void clear_depth();
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
    void parse_from_options (Options const & opt, bool check=true);

    /// get range in lon-lat coords
    dRect range() const;
    /// get length in m
    double length() const;

    operator dLine() const;
};

#ifdef SWIG
%template(vector_g_trackpoint) std::vector<g_trackpoint>;
%template(vector_g_track)  std::vector<g_track>;
#endif  // SWIG

#endif

