#ifndef POINTSET_H
#define POINTSET_H

#include "point.h"
#include <set>

// Доступ к 8 соседям
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

// Доступ к 4 соседям
Point<int> adj4(Point<int> p, int dir){
  switch(dir%4){
    case 0: return Point<int>(p.x  ,p.y-1);
    case 1: return Point<int>(p.x+1,p.y  );
    case 2: return Point<int>(p.x  ,p.y+1);
    case 3: return Point<int>(p.x-1,p.y  );
  }
}

// Проверка, являются ли точки соседними.
// Если да, то возвращается направление от первой ко второй,
// если нет, то -1
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


// построение границы множества точек
template <typename Container1, typename Container2>
Container1 border(const Container2 & points){
  Container1 ret;
  typename Container2::iterator i;
  for (i=points.begin();i!=points.end();i++){
    for (int d=0; d<8; d++)
      ret.insert(adj8(*i, d));
  }
  for (i=points.begin();i!=points.end();i++){
    ret.erase(*i);
  }
  return ret;
}

template <typename Container>
std::ostream & operator<< (std::ostream & s, const Container<Point<> > & points){
  s << "Points(\n";

  typename Container::iterator i;
  for (i=points.begin();i!=points.end();i++){
    s << "  " << *i << "\n";
  }

  s << ")\n";
}

/*

  // центр множества точек
  Point<int> center(const std::set<Point<int> >& points);

  // добавить точку в множество points, соответствующим образом
  // изменить его границу bord
  void add_pb(const Point<int>& p, std::list<Point<int> >& plist, std::set<Point<int> >& bord);
  void add_pb(const Point<int>& p, std::set<Point<int> >& points, std::set<Point<int> >& bord);
*/
#endif
