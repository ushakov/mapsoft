#ifndef TRACER_H
#define TRACER_H

#include "srtm3.h"
#include <2d/point.h>
#include <2d/rect.h>
#include <map>
#include <set>

// Трассирование речек для водосбора некоторой заданной точки. То же для хребтов.

class tracer {

public:
  std::set<iPoint> done; // the whole area proccessed (optimisation needed?)
  std::map<iPoint, double> res; // raw result
  srtm3 S;

  int max_points;
  int max_height_rv;
  int max_height_mt;
  double min_area;
  bool river;
  iPoint start_pt;

  tracer(const char *dir = "./", int cache_size=10);

  void trace_river(const dPoint & p);
  void trace_mount(const dPoint & p);
  void trace();

  // есть ли сток из направления dir в данную точку?
  bool is_flow(const iPoint &p, int dir);

  // recursively get area
  double get_a(iPoint p0);

  iRect get_range() const;

  std::set<iPoint> get_border() const;

};

#endif
