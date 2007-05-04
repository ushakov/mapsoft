#ifndef POLY_H
#define POLY_H

#include <iostream>
#include <list>
#include <map>
#include <set>
#include <algorithm>

#include "point.h"

// ����� -- ������ �����

template <typename T> 
struct Line : std::list<Point<T> > {
};


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
  double k = p12.x*p34.y - p12.y*p34.x;
  double ka = p13.x*p34.y - p13.y*p34.x;
  double kb = p13.x*p12.y - p13.y*p12.x;
  if (k==0) throw 0; // ������������ �����(?)
  return Point<double>(ka/k, kb/k);
}

// ����������� ���� ��������
// ������� �������� ������ �����, �� �� �������� ���������
// ���� ������� �� ������������ - ��������� ����������

template <typename T> 
Point<T> find_cross(const Point<T> & p1, const Point<T> & p2, 
                    const Point<T> & p3, const Point<T> & p4){
  Point<double> ab = ind_cross_ab(p1,p2,p3,p4);
  if ((ab.x<0)||(ab.x>=1)) throw 0; // ����������� - �� ��������� �������
  if ((ab.y<0)||(ab.y>=1)) throw 0; // ����������� - �� ��������� �������
  return Point<T>((T)(p1.x+ab.x*(p2.x-p1.x)), (T)(p1.y+ab.x*(p2.y-p1.y)));
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

template<typename X, typename Y>
bool sort_pairs(const std::pair<X,Y> & p1, const std::pair<X,Y> & p2)
     {return p1.first < p2.first;}


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
      typedef std::pair<typename Line<T>::iterator, typename Line<T>::iterator> crossing_t;
      typedef std::list<crossing_t> crossings_t;
      crossings_t crossings;

      // ������� ��� �����������.
      // ��� ������� ����� ���� ����������� c� ����� ���������� ��������.
      // ��������� � ��� ����� ����������� �����, � � �������� ����� - 
      // �����, ���������� ������ �������.

      typename Line<T>::iterator p1,p2=l.begin();

      while (p2!=l.end()){
	p1=p2; p2++; if (p2==l.end()) break;

        typedef std::pair<double, typename Line<T>::iterator> my_as_pt;
        std::vector<my_as_pt> as;

        typename Line<T>::iterator p3,p4=p2; 
        p4++; //(��� �������� ����� �� ������������ :))
        while (p4!=l.end()){
	  p3=p4; p4++; if (p4==l.end()) break;

	  try {
            Point<double> ab = find_cross_ab(*p1,*p2,*p3,*p4);
            if ((ab.x<0)||(ab.x>=1)||(ab.y<0)||(ab.y>=1)) continue;
            // ������� ����� �� ������ ����� (���� ��� �� �� ��������� �����)
            // ������� ������ � as
            if (ab.y==0){as.push_back(my_as_pt(ab.x,p3));}
            else {
              as.push_back(my_as_pt(ab.x, 
                l.insert(p4,
                  Point<T>((T)(p3->x + ab.y*(p4->x - p3->x)), 
                           (T)(p3->y + ab.y*(p4->y - p3->y))))
                )
              );
            }
          } catch(int i){}
        }
	// ������ ����������� �����������, ������� �� �� ������ �����,
        // ������� � crossings �������� � ����� ����� �� ������ � �������:
        std::sort(as.begin(), as.end(), sort_pairs<double, typename Line<T>::iterator>);

        for (typename std::vector<my_as_pt>::const_iterator p=as.begin(); p!=as.end(); p++){
          typename Line<T>::iterator i1 = l.insert(p2, 
            Point<T>((T)(p1->x + p->first*(p2->x - p1->x)),
                     (T)(p1->y + p->first*(p2->y - p1->y)))
          );
	    // ������� ���� � �������
          crossings.push_back(crossing_t(i1,p->second));
          crossings.push_back(crossing_t(p->second,i1));
        }
      }


      // �������� ����� ����� �� �������� ������

      // ���� ����������� ��� - �� � ���...
      if (crossings.size()==0) this->std::list<Line<T> >::push_back(l);

      while (crossings.size()>0){
	Line<T> newline;

	// ����� �������
        crossing_t cr = *crossings.begin();
        crossings.pop_front();

	// �� ������ �� cr.second � ������ �� cr.first
        newline.push_back(*(cr.second));
        while (1){
          cr.second++;
          if (cr.second == l.end()) cr.second = l.begin();
          newline.push_back(*(cr.second));
          if (cr.first==cr.second) break;
          // ����� �� �� �� ������ �����������?
          for (typename crossings_t::iterator f=crossings.begin(); f!=crossings.end(); f++){
            if (f->first == cr.second){
              cr.second = f->second; // �������� �� ������ �����
              crossings.erase(f);
              break;
            }
          }
        }
        this->std::list<Line<T> >::push_back(newline);
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
