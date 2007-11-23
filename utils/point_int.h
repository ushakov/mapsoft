#ifndef POINT_INT_H
#define POINT_INT_H

#include <set>
#include "point.h"
#include "line.h"

// построение границы множества точек
std::set<Point<int> > border(const std::set<Point<int> >& pset);
// добавить точку в множество pset, соответствующим образом
// изменить его границу bord
int add_pb(const Point<int>& p, std::set<Point<int> >& pset, std::set<Point<int> >& bord);
// ппреобразовать множество точек в линию границы
std::vector<Line<double> > pset2line (const std::set<Point<int> >& pset);

#endif
