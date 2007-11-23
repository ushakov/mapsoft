#ifndef POINT_INT_H
#define POINT_INT_H

#include <set>
#include "point.h"
#include "line.h"

// ���������� ������� ��������� �����
std::set<Point<int> > border(const std::set<Point<int> >& pset);
// �������� ����� � ��������� pset, ��������������� �������
// �������� ��� ������� bord
int add_pb(const Point<int>& p, std::set<Point<int> >& pset, std::set<Point<int> >& bord);
// �������������� ��������� ����� � ����� �������
std::vector<Line<double> > pset2line (const std::set<Point<int> >& pset);

#endif
