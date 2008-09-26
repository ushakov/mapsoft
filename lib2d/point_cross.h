#ifndef POINT_CROSS_H
#define POINT_CROSS_H

#include "point.h"
#include "point_utils.h"

// ����������� ������, ���������� ��������� p1-p2 � p3-p4.
// ��� ����� ����������� p ������������ ��� �����:
// |p-p1|/|p2-p1| � |p-p3|/|p4-p3|
// ���� ������ ����������� - ��������� ����������.

template <typename T>
Point<double> find_cross_ab(const Point<T> & p1, const Point<T> & p2,
                    const Point<T> & p3, const Point<T> & p4){
  Point<T> p12 = p2-p1;
  Point<T> p34 = p4-p3;
  Point<T> p13 = p3-p1;

  T k  =  p12.x*p34.y - p12.y*p34.x;
  T ka =  p13.x*p34.y - p13.y*p34.x;
  T kb =  p13.x*p12.y - p13.y*p12.x;

  if (k==0) throw 0; // ������������ �����(?)
  return Point<double>(ka/(double)k, kb/(double)k);
}

// ����������� ���� ��������
// ������� �������� ������ �����, �� �� �������� ���������
// ���� ������� �� ������������ - ��������� ����������

// TODO: ������������ ���������� ������ ��������, ������� �� ����� ������!
// ������ ��� ��������� ����������������� ��-�� ���� ��������� ������ ���
// ��������� ����� �� ������������� ��������������!
// (��������, ����� �������� srtm2fig

template <typename T>
Point<T> find_cross(const Point<T> & p1, const Point<T> & p2,
                    const Point<T> & p3, const Point<T> & p4){
  Point<double> ab = find_cross_ab(p1,p2,p3,p4);
  if ((ab.x<0)||(ab.x>=1)) throw 0; // ����������� - �� ��������� �������
  if ((ab.y<0)||(ab.y>=1)) throw 0; // ����������� - �� ��������� �������
  return Point<T>(Point<double>(p1)+ab.x*Point<double>(p2-p1));
}

// ���������� ���������� ����� ������ � ��������
template <typename T>
double find_dist(const Point<T> & p,
                 const Point<T> & p1, const Point<T> & p2){

  double  ll = pdist(p1,p2);
  if (ll==0) return pdist(p,p1); // ������� ������� �����

  Point<double> vec = Point<double>(p2-p1)/ll;

  double l1 = pdist(p,p1);
  double l2 = pdist(p,p2);
  double ret = l1<l2 ? l1:l2;

  double prl = pscal(Point<double>(p-p1), vec);

  if ((prl>=0)&&(prl<=ll)){ // �������� ������ �� �������
    Point<double> pc = Point<double>(p1) + vec * prl;
    double lc=pdist(Point<double>(p),pc);
    ret= ret<lc ? ret:lc;
  }
  return ret;
}

// ���������� ���������� ����� ����� ���������
template <typename T>
double find_dist(const Point<T> & p1, const Point<T> & p2,
                 const Point<T> & p3, const Point<T> & p4){
  return min( min(find_dist(p1,p3,p4), find_dist(p2,p3,p4)),
              min(find_dist(p3,p1,p2), find_dist(p4,p1,p2)));
}

#endif /* POINT_CROSS_H */
