#ifndef GOBJ_TRK_H
#define GOBJ_TRK_H

#include "gred/gobj.h"
#include "geo/geo_data.h"

/// Растровый слой для показа треков

class GObjTRK
#ifndef SWIG
  : public GObj
#endif  // SWIG
{
private:
  g_track * data; // указатель на геоданные
  Options opt;
  iRect myrange;

public:
  GObjTRK (g_track * _data, const Options & o = Options());

  void set_opt(const Options & o);
  Options get_opt(void) const;

  /// Refresh layer.
  void refresh();

  /// Get some reasonable reference.
  g_map get_myref() const;

  /// Draw on image.
  int draw(iImage & image, const iPoint & origin);

  int find_trackpoint (iPoint pt, int radius = 3);

  std::vector<int> find_trackpoints (const iRect & r);

  // Поиск трека. Находится сегмент, в который тыкают, возвращается
  // первая точка сегмента (0..size-2).
  int find_track (iPoint pt, int radius = 3);

  /// Get pointer to the data object.
  g_track * get_data() const;

  /// Get pointer to the n-th point.
  g_trackpoint * get_pt(const int n) const;

  /// Get layer range.
  iRect range() const;
};

#endif
