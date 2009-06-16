%include "../../core/lib2d/point.i"
%include "../../core/utils/options.i"
%include "std_vector.i"

%{
#include "geo_data.h"
%}

%template(g_point) Point<double>;
%template(vector_g_waypoint) std::vector<g_waypoint>;
%template(vector_g_trackpoint) std::vector<g_trackpoint>;
%template(vector_g_refpoint) std::vector<g_refpoint>;

%include "geo_data.h"
