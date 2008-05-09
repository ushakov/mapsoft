#ifndef POINT_INT_H
#define POINT_INT_H

#include <set>
#include "point.h"
#include "line.h"

/// Доступ к 8 соседним точкам
Point<int> adjacent(const Point<int> &p, const int dir);

/// Проверка, являются ли точки соседними.
/// Если да, то возвращается направление от p1 к p2,
/// если нет, то -1
int isadjacent(const Point<int> & p1, const Point<int> & p2);

/// построение границы множества точек
std::set<Point<int> > border(const std::set<Point<int> >& pset);

/// добавить точку в множество pset, соответствующим образом
/// изменить его границу bord
int add_pb(const Point<int>& p, std::set<Point<int> >& pset, std::set<Point<int> >& bord);

/// преобразовать множество точек в линию границы
std::list<Line<double> > pset2line (const std::set<Point<int> >& pset);


#endif /* POINT_INT_H */
