#ifndef POLY_H
#define POLY_H

#include <iostream>
#include <list>
#include <map>
#include <set>

#include "point.h"

// ����� -- ������ �����

template <typename T> 
struct Line : std::list<Point<T> > {
};

// ��������� ������ ����������� ���� ��������
// ���� ������� �� ������������ - ��������� ����������!
// ������� �������� ������ �����, �� �� �������� ���������

template <typename T> 
Point<T> find_cross(const Point<T> & p1, const Point<T> & p2, 
                    const Point<T> & p3, const Point<T> & p4){
  Point<T> p12 = p2-p1;
  Point<T> p43 = p3-p4;
  Point<T> p14 = p4-p1;
  double k1 = p12.x*p43.y - p12.y*p43.x;
  double k2 = p14.x*p43.y - p14.y*p43.x;
  if (k1==0) throw; // ������������ �����(?)
  double a = k2/k1;
  if ((a<0)||(a>=1)) throw 0; // ����������� - �� ��������� �������
  return Point<T>((T)(p1.x+a*p12.x), (T)(p1.y+a*p12.y));
}

// ���������� �������������:
// ���� ��������� ��������� ��������. 
// "�������������" ������ - ��������� �� ������� �������,
// "�������������" - ������. 

// ������ � ����������������� ����������� �� ��������� 
// �������� ������ ������. (�������� ��� ��� ���! ������, ����
// ���-�� ��������� ����������?)

// ����� ��������� ����������� ��������������, 
// ���� ����� ������������ �� ������������� ��������
// ���� ����� �� ������������ �� ������������� �������� �������.

// ������� �������: ������������� ������ ������ ��������� ���� �����
// ������ ����, ������������� - ��� ����.
// ("���������" ������ ��������� ����� ��� ���� :))
// 

template <typename T> 
struct Polygon : std::list<Line<T> > {

    void push_back(Line<T> l){
      // ���������� �������.
      // ���� � ������� ������ ���� ����� - ��� �� ����� �����
      if (l.size()<3) return;
      // ��������� ����� ������ ����� �� �� ����������, ��� � ������.
      if (*l.begin() != *l.rbegin()) l.push_back(*l.begin());

      // ������ � ����������������� ����� ������� �� ���������.

      // ����� ��� ������� �����������
      typedef std::map<typename Line<T>::iterator, typename Line<T>::iterator> crossings_t;
      crossings_t crossings;

      // ������� ��� �����������:
      typename Line<T>::iterator p1,p2,p3,p4;
      for (p1 = l.begin(); p1!=l.end(); p1++){
        p2=p1; p2++; if (p2==l.end()) continue;
        for (p3 = p2; p3!=l.end(); p3++){
          p4=p3; p4++; if (p4==l.end()) continue;
          try {
            Point<T> p = find_cross(*p1,*p2,*p3,*p4);
	    typename Line<T>::iterator i1 = (p==*p1)? p1 : l.insert(p1,p);
	    typename Line<T>::iterator i2 = (p==*p3)? p3 : l.insert(p3,p);
            crossings[i1] = i2;
            crossings[i2] = i1;
          } catch(int i){}
        }
      }
      
      while (crossings.size()>0){
	// �������� ����� ����� �� �������� ������
	Line<T> newline;

        std::pair<typename Line<T>::iterator, typename Line<T>::iterator> 
          cr = *crossings.begin();
          crossings.erase(crossings.begin());
        // ���� �� ������ � ���� �� �����������
        while (cr.first!=cr.second){
          // ����� �� �� �� ������ �����������?
          typename crossings_t::iterator f = crossings.find(cr.first);
          if (f!=crossings.end()){
            cr.first = f->second; // �������� �� ������ �����
            crossings.erase(f);
          }
          // ����� �� �� �� ������ �����?
	  if (cr.first == l.end()) cr.first=l.begin();
          newline.push_back(*(cr.first));
        }
        this->push_back(newline);
      }
    }
};


// ����������� ����� �������
// ��� ����� ���� ����� ��������:
// - ���� ��������� xdy ��� ydx
// - ������� ����� � ����������� ����������� � ����������
//   ������� �������� �� ��������� � ���������� �����
// (�� � ����������������� ��� ����� �������� �����������!)
template <typename T> 
int polygon_sign(const Polygon<T> & p){

}
// ����������� �� ����� ��������������
// - ��� ������� ������� ����� ��������� �����, 

template <typename T> 
bool pt_in_polygon(const Polygon<T> & plg, const Point<T> & p){
}

// ��� ���� �������� p1-p2 � p3-p4 ����� ����� �����������


#endif /* POLY_H */
