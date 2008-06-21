#ifndef LINE_POLYCROP_H
#define LINE_POLYCROP_H

#include "line.h"
#include "point_utils.h"
#include "point_cross.h"

/// ��������� ����� �� ��������������. 
/// ��� ����� ��������������� �������� �� ������, ������� �������� ������������� :(

// ����� �� ����� � �������������� poly
template<typename T>
bool test_pt (const Point<double> & pt, const T & poly){
  double a = 0;
  typename T::const_iterator p1,p2;
  for (int i = 0; i<poly.size(); i++){
    Point<double> v1 = Point<double>(poly[i]) - pt;
    Point<double> v2 = Point<double>(poly[(i+1)%poly.size()]) - pt;
    double dd = pdist(v1)*pdist(v2);
    if (dd==0) return true;

    double s = v1.x*v2.y - v1.y*v2.x;
    double c = pscal(v1,v2)/dd;
    if (fabs(c)>=1) {continue;}

    if (s<0) a+=acos(c); else a-=acos(c);
  }

  return (fabs(a)>M_PI);
}

// �������� ��� �����, ��������/�� �������� � ������������� cutter � �������� �� � lines1
template<typename T>
void crop_lines(std::list<T> & lines,
                std::list<T> & lines1,
                const T & cutter, bool cutouter){
  for (int j = 0; j<cutter.size(); j++){
    for (typename std::list<T>::iterator l = lines.begin(); l!=lines.end(); l++){
      for (int i = 0; i<l->size()-1; i++){
        Point<double> pt;
        try { pt = find_cross((*l)[i], (*l)[i+1], cutter[j], cutter[(j+1)%cutter.size()]); }
        catch (int n) {continue;}
        // ��������� ����� �� ���, ��� ������������ ����� �������� ����� l
        T l1(*l); l1.clear(); // �� ����� �������� �� ������ � �������, �� � � ���������!
        for (int k=0; k<=i; k++) l1.push_back((*l)[k]);
        l1.push_back(pt);
        lines.insert(l, l1);
        // �� *l ������� ��� ����� �� i-1-�
        l->erase(l->begin(), l->begin()+i);
        *(l->begin()) = pt;
        // ���������� �� ������� ����� ���������� �����
        i=1;
      }
    }
  }
  // ������ ���������� �� �����, ������� �� �������� � ������ �����
  typename std::list<T>::iterator l=lines.begin();
  while (l!=lines.end()){
    if (l->size()==0) {l=lines.erase(l); continue;}
    // ��������� ����� ����� ������ - ����� ����� ���.
    int sum=0;
    for (int i = 1; i<l->size(); i++){
      sum+= (cutouter xor test_pt(Point<double>((*l)[i]+(*l)[i-1])/2.0, cutter))? -1:1;
    }
    if (sum<0) {lines1.push_back(*l);  l=lines.erase(l); continue;}
    l++;
  }
}

#endif /* LINE_POLYCROP_H */
