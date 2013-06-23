#ifndef GOBJ_WPT_H
#define GOBJ_WPT_H

#include "gred/gobj.h"
#include "geo/geo_data.h"

/// Растровый слой для показа точек

class GObjWPT
#ifndef SWIG
  : public GObj
#endif  // SWIG
{
private:
  g_waypoint_list * data; // указатель на геоданные
  iRect myrange;

public:

  GObjWPT (g_waypoint_list * _data);

  /// Refresh layer.
  void refresh();

  /// Set some reasonable reference.
  g_map get_myref() const;

  /// Draw on image.
  int draw(iImage & image, const iPoint & origin);

  int find_waypoint (iPoint pt, int radius = 3);

  std::vector<int> find_waypoints (const iRect & r);

  /// Get pointer to the data object.
  g_waypoint_list * get_data() const;

  /// Get pointer to the n-th point.
  g_waypoint * get_pt(const int n) const;

  /// Get layer range.
  iRect range () const;
};


#endif
