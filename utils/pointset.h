#ifndef POINTSET_H
#define POINTSET_H

#include "point.h"
#include <set>

// ������ � 8 �������
Point<int> adj8(Point<int> p, int dir){
  switch(dir%8){
    case 0: return Point<int>(p.x-1,p.y-1);
    case 1: return Point<int>(p.x  ,p.y-1);
    case 2: return Point<int>(p.x+1,p.y-1);
    case 3: return Point<int>(p.x+1,p.y  );
    case 4: return Point<int>(p.x+1,p.y+1);
    case 5: return Point<int>(p.x  ,p.y+1);
    case 6: return Point<int>(p.x-1,p.y+1);
    case 7: return Point<int>(p.x-1,p.y  );
  }
}

// ������ � 4 �������
Point<int> adj4(Point<int> p, int dir){
  switch(dir%4){
    case 0: return Point<int>(p.x  ,p.y-1);
    case 1: return Point<int>(p.x+1,p.y  );
    case 2: return Point<int>(p.x  ,p.y+1);
    case 3: return Point<int>(p.x-1,p.y  );
  }
}

// ��������, �������� �� ����� ���������.
// ���� ��, �� ������������ ����������� �� ������ �� ������,
// ���� ���, �� -1
int isadj8(Point<int> p1, Point<int> p2){
  for (int i = 0; i<8; i++){
    if (adj8(p1,i)==p2) return i;
  } return -1;
}
int isadj4(Point<int> p1, Point<int> p2){
  for (int i = 0; i<4; i++){
    if (adj4(p1,i)==p2) return i;
  } return -1;
}


// ���������� ���� �� ��������...

  // ���������� ������� ��������� �����
  template <typename Container1, typename Container2>
  Container1 border(const Container2 & pset){
    Container1 ret;
    Container2::iterator i, b=pset.begin(), e=pset.end();
    for (i=b;i!=e;i++){
      for (int d=0; d<8; d++)
        ret.insert(adj8(*i, d));
    }
    for (i=b;i!=e;i++){
      ret.erase(*i);
    }
    return ret;
  }

/*

  // ����� ��������� �����
  Point<int> center(const std::set<Point<int> >& pset);

  // �������� ����� � ��������� pset, ��������������� �������
  // �������� ��� ������� bord
  void add_pb(const Point<int>& p, std::list<Point<int> >& plist, std::set<Point<int> >& bord);
  void add_pb(const Point<int>& p, std::set<Point<int> >& pset, std::set<Point<int> >& bord);
*/
#endif
