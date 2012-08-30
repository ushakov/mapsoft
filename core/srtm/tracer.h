#ifndef TRACER_H
#define TRACER_H

#include "srtm3.h"
#include <2d/point.h>
#include <2d/rect.h>
#include <map>
#include <set>
#include <string>

// Трассирование речек для водосбора некоторой заданной точки. То же для хребтов.

class tracer {

public:
  std::set<iPoint> done; // the whole area proccessed (optimisation needed?)
  std::map<iPoint, double> res; // raw result
  srtm3 S;

  int max_points;
  int max_height;
  double min_area;
  bool river;
  iPoint start_pt;

  tracer(const std::string & dir = std::string(), int cache_size=10);
  void trace(const dPoint & p, bool down);

  // есть ли сток из направления dir в данную точку?
  bool is_flow(const iPoint &p, int dir);

  // recursively get area
  double get_a(iPoint p0);

  iRect get_range() const;

  std::set<iPoint> get_border() const;

};

#endif
