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

Point<int> my_crn (int k){
  k%=4;
  return Point<int>(k/2, (k%3>0)?1:0);
}


std::vector<Line<double> > pset2line (const std::set<Point<int> >& pset){
  std::vector<Line<double> > ret, ret1;
  //добавляем все обходы точек
  for (std::set<Point<int> >::const_iterator i = pset.begin(); i!=pset.end(); i++){
    for (int k = 0; k<4; k++){
      Line<double> side;
      Point<double> dp(0.5,0.5);
      side.push_back(Point<double>(*i)+Point<double>(my_crn(k)) - dp);
      side.push_back(Point<double>(*i)+Point<double>(my_crn(k+1)) - dp);
      ret.push_back(side);
    }
  }
  ret1 = merge(ret,0.1);
  return ret1;
}
