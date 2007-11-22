#include "point_int.h"

std::set<Point<int> > border(const std::set<Point<int> >& pset){
  std::set<Point<int> > ret;
  for (std::set<Point<int> >::const_iterator it = pset.begin();
                                 it != pset.end(); it++){
    for (int i=0; i<8; i++)
      ret.insert(it->adjacent(i));
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
    Point<int> p2 = p.adjacent(i);
    if (pset.find(p2)==pset.end()) bord.insert(p2);
  }
  return 1;
}
