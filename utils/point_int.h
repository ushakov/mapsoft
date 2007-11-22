#ifndef POINT_INT_H
#define POINT_INT_H

#include <set>
#include "point.h"

// построение границы множества точек
std::set<Point<int> > border(const std::set<Point<int> >& pset);
// добавить точку в множество pset, соответствующим образом
// изменить его границу bord
int add_pb(const Point<int>& p, std::set<Point<int> >& pset, std::set<Point<int> >& bord);

#endif
