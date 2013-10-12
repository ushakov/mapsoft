#ifndef GOBJ_TRK_H
#define GOBJ_TRK_H

#include "gobj_geo.h"
#include "geo/geo_data.h"

/// Растровый слой для показа треков

class GObjTRK
#ifndef SWIG
  : public GObjGeo
#endif  // SWIG
{
private:
  g_track * data; // указатель на геоданные
  iRect myrange;

public:
  GObjTRK (g_track * _data, const Options & o = Options()):
    data(_data){ set_opt(o); }
  void refresh(){ /// Refresh layer.
      myrange = iRect(rect_pump(cnv.bb_bck(data->range()), 1.0));} 
  int draw(iImage & image, const iPoint & origin); /// Draw on image.

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
