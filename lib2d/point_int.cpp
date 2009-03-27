#include "point_int.h"
#include "line_utils.h"

// Доступ к 8 соседним точкам
Point<int> adjacent(const Point<int> &p, const int dir){
  switch(dir%8){
    case 0: return Point<int>(p.x-1,p.y-1);
    case 1: return Point<int>(p.x,  p.y-1);
    case 2: return Point<int>(p.x+1,p.y-1);
    case 3: return Point<int>(p.x+1,p.y  );
    case 4: return Point<int>(p.x+1,p.y+1);
    case 5: return Point<int>(p.x,  p.y+1);
    case 6: return Point<int>(p.x-1,p.y+1);
    case 7: return Point<int>(p.x-1,p.y  );
  }
}

// Проверка, являются ли точки соседними.
// Если да, то возвращается направление от p1 к p2,
// если нет, то -1
int isadjacent(const Point<int> & p1, const Point<int> & p2){
  for (int i = 0; i<8; i++){
    if (adjacent(p1, i) == p2) return i;
  }
  return -1;
}


std::set<Point<int> > border(const std::set<Point<int> >& pset){
  std::set<Point<int> > ret;
  for (std::set<Point<int> >::const_iterator it = pset.begin();
                                 it != pset.end(); it++){
    for (int i=0; i<8; i++)
      ret.insert(adjacent(*it, i));
  }
  for (std::set<Point<int> >::const_iterator it = pset.begin();
                                 it != pset.end(); it++){
    ret.erase(*it);
  }
  return ret;
}


int add_pb(const Point<int>& p, std::set<Point<int> >& pset, std::set<Point<int> >& bord){
  if (pset.find(p)!=pset.end()) return 0; // точка уже есть
  pset.insert(p);
  bord.erase(p);
  for (int i=0; i<8; i++){
    Point<int> p2 = adjacent(p, i);
    if (pset.find(p2)==pset.end()) bord.insert(p2);
  }
  return 1;
}

Point<int> my_crn (int k){
  k%=4;
  return Point<int>(k/2, (k%3>0)?1:0);
}


MultiLine<double> pset2line (const std::set<Point<int> >& pset){
  MultiLine<double> ret, ret1;
  //добавляем все обходы точек
  for (std::set<Point<int> >::const_iterator i = pset.begin(); i!=pset.end(); i++){
    for (int k = 0; k<4; k++){
      Line<double> side;
      Point<double> dp(0.5,0.5);
      side.push_back(Point<double>(*i+my_crn(k)) - dp);
      side.push_back(Point<double>(*i+my_crn(k+1)) - dp);
      // у нас уже может быть такая линия, пройденная в обратном направлении.
      // merge такую пару отловит и удалит, но это долгая процедура.
      // поэтому посмотрим здесь:
      // upd. это не сильно все убыстрило...
      bool ispair=false;
      for (MultiLine<double>::iterator i = ret.begin(); i!=ret.end(); i++){
        if (i->isinv(side)){
           ispair = true;
           ret.erase(i);
           break;
        }
      }
      if (!ispair) ret.push_back(side);
    }
  }
  merge(ret,0.1);
  return ret;
}
