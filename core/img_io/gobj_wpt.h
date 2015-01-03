#ifndef GOBJ_WPT_H
#define GOBJ_WPT_H

#include "gobj_geo.h"
#include "geo/geo_data.h"

/// Растровый слой для показа точек

class GObjWPT
#ifndef SWIG
  : public GObjGeo
#endif  // SWIG
{
private:
  g_waypoint_list * data; // указатель на геоданные
  iRect myrange;

public:

  GObjWPT (g_waypoint_list * _data, const Options & o = Options()):
    data(_data){set_opt(o);}

  void   refresh();

  int draw(iImage & image, const iPoint & origin); /// Draw on image.

  int find_waypoint (iPoint pt, int radius = 3);

  std::vector<int> find_waypoints (const iRect & r);

  /// Get pointer to the data object.
  g_waypoint_list * get_data() const {return data;}

  /// Get pointer to the n-th point.
  g_waypoint * get_pt(const int n) const {return &(*data)[n];}

  /// Get layer range.
  iRect range () const {return myrange;}
};


#endif
