#ifndef POINT_INT_H
#define POINT_INT_H

#include <set>
#include "point.h"
#include "line.h"

///\addtogroup lib2d
///@{
///\defgroup point_int
///@{

/// Доступ к 8 соседним точкам
iPoint adjacent(const iPoint &p, const int dir);

/// Проверка, являются ли точки соседними.
/// Если да, то возвращается направление от p1 к p2,
/// если нет, то -1
int isadjacent(const iPoint & p1, const iPoint & p2);

/// построение границы множества точек
std::set<iPoint> border(const std::set<iPoint>& pset);

/// добавить точку в множество pset, соответствующим образом
/// изменить его границу bord
int add_pb(const iPoint& p, std::set<iPoint>& pset, std::set<iPoint>& bord);

/// преобразовать множество точек в линию границы
dMultiLine pset2line (const std::set<iPoint>& pset);


#endif /* POINT_INT_H */
