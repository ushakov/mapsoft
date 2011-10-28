#ifndef POINT_H
#define POINT_H

#include <vector>
#include <set>
#include <list>

//количество соседей
const int point_adjs = 8;

// Абстрактная точка с целыми координатами x,y
//   нет проверки диапазона и привязки к конкретной картинке
struct point{
  int x,y;  

  point(int _x, int _y){ x=_x; y=_y;}
  point(){ x=0; y=0;}

  bool operator== (const point& p) const
   {return ((x==p.x)&&(y==p.y));}
  bool operator!= (const point& p) const
   {return ((x!=p.x)||(y!=p.y));}
  bool operator< (const point& p) const
   {return (x<p.x) || ((x==p.x)&&(y<p.y));}
  bool operator> (const point& p) const
   {return (x>p.x) || ((x==p.x)&&(y>p.y));}

  // Доступ к 8 соседним точкам
  point adjacent(int dir) const {
    switch(dir%8){
      case 0: return point(x-1,y-1);
      case 1: return point(  x,y-1);
      case 2: return point(x+1,y-1);
      case 3: return point(x+1,  y);
      case 4: return point(x+1,y+1);
      case 5: return point(  x,y+1);
      case 6: return point(x-1,y+1);
      case 7: return point(x-1,  y);
    }
  }
  // Проверка, являются ли точка соседней.
  // Если да, то возвращается направление на нее,
  // если нет, то -1
  int isadjacent(point p){
    for (int i = 0; i<point_adjs; i++){
      if (adjacent(i)==p) return i;
    } return -1;
  }

};


// построение границы множества точек
std::set<point> border(const std::set<point>& pset);
std::set<point> border(const std::list<point>& pset);
// центр множества точек
point center(const std::set<point>& pset);

// построение плоскости
//std::set<point> plane(const point& p, int(*f)(int,int) );

// добавить точку в множество pset, соответствующим образом
// изменить его границу bord
void add_pb(const point& p, std::list<point>& plist, std::set<point>& bord);
void add_pb(const point& p, std::set<point>& pset, std::set<point>& bord);

#endif
