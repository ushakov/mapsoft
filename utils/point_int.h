#ifndef POINT_INT_H
#define POINT_INT_H

#include <set>
#include "point.h"

// ���������� ������� ��������� �����
std::set<Point<int> > border(const std::set<Point<int> >& pset);
// �������� ����� � ��������� pset, ��������������� �������
// �������� ��� ������� bord
int add_pb(const Point<int>& p, std::set<Point<int> >& pset, std::set<Point<int> >& bord);

#endif
