#ifndef POINT_INT_H
#define POINT_INT_H

#include <set>
#include "point.h"
#include "line.h"

/// ������ � 8 �������� ������
Point<int> adjacent(const Point<int> &p, const int dir);

/// ��������, �������� �� ����� ���������.
/// ���� ��, �� ������������ ����������� �� p1 � p2,
/// ���� ���, �� -1
int isadjacent(const Point<int> & p1, const Point<int> & p2);

/// ���������� ������� ��������� �����
std::set<Point<int> > border(const std::set<Point<int> >& pset);

/// �������� ����� � ��������� pset, ��������������� �������
/// �������� ��� ������� bord
int add_pb(const Point<int>& p, std::set<Point<int> >& pset, std::set<Point<int> >& bord);

/// ������������� ��������� ����� � ����� �������
std::list<Line<double> > pset2line (const std::set<Point<int> >& pset);


#endif /* POINT_INT_H */
