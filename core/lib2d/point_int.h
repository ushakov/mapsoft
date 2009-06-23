#ifndef POINT_INT_H
#define POINT_INT_H

#include <set>
#include "point.h"
#include "line.h"

/// Доступ к 8 соседним точкам
iPoint adjacent(const Point<int> &p, const int dir);

/// Проверка, являются ли точки соседними.
/// Если да, то возвращается направление от p1 к p2,
/// если нет, то -1
int isadjacent(const iPoint & p1, const Point<int> & p2);

/// построение границы множества точек
std::set<iPoint> border(const std::set<Point<int> >& pset);

/// добавить точку в множество pset, соответствующим образом
/// изменить его границу bord
int add_pb(const iPoint& p, std::set<Point<int> >& pset, std::set<Point<int> >& bord);

/// преобразовать множество точек в линию границы
dMultiLine pset2line (const std::set<iPoint>& pset);


#endif /* POINT_INT_H */
