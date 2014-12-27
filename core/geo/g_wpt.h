#ifndef G_WPT_H
#define G_WPT_H

#include <vector>
#include <string>

#include "geo_types.h"
#include "2d/rect.h"
#include "options/options.h"

#include "options/m_time.h"

using namespace std;

/// single waypoint
struct g_waypoint : dPoint {
    string  name;
    string  comm;
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
    int          color;
    int          bgcolor;

    g_waypoint();
    Options to_options() const;
    void parse_from_options(Options const & opt, bool check=true);

    bool have_alt() const;
    void clear_alt();
};

/// waypoint list
struct g_waypoint_list : vector<g_waypoint>{

    string symbset; /// garmin symbol set -- not used now
    string comm;

    g_waypoint_list();

    /// convert waypoint_list values to Options object
    Options to_options () const;

    /// set waypoint_list values from Options object
    void parse_from_options (Options const & opt, bool check=true);

    /// get range in lon-lat coords
    dRect range() const;

};

#ifdef SWIG
%template(vector_g_waypoint) vector<g_waypoint>;
%template(vector_g_waypoint_list)  vector<g_waypoint_list>;
#endif  // SWIG

#endif

