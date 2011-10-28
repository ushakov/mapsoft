#include <set>
//#include <queue>
#include "point.h"

std::set<point> border(const std::set<point>& pset){
  std::set<point> ret;
  for (std::set<point>::const_iterator it = pset.begin();
                                 it != pset.end(); it++){
    for (int i=0; i<point_adjs; i++)
      ret.insert(it->adjacent(i));
  }
  for (std::set<point>::const_iterator it = pset.begin();
                                 it != pset.end(); it++){
    ret.erase(*it);
  }
  return ret;
}

std::set<point> border(const std::list<point>& plist){
  std::set<point> ret;
  for (std::list<point>::const_iterator it = plist.begin();
                                  it != plist.end(); it++){
    for (int i=0; i<point_adjs; i++)
      ret.insert(it->adjacent(i));
  }
  for (std::list<point>::const_iterator it = plist.begin();
                                  it != plist.end(); it++){
    ret.erase(*it);
  }
  return ret;
}


point center(const std::set<point>& pset){
  point ret(0,0);
  int n;
  if (pset.empty()) return ret;

  for (std::set<point>::iterator it = pset.begin();
                                 it != pset.end(); it++){
    ret.x += it->x;
    ret.y += it->y;
    n++;
  }
  ret.x/=n;
  ret.y/=n;
  return ret;
};

bool isinlist(std::list<point>& L, const point& p){
  for (std::list<point>::iterator it = L.begin(); it != L.end(); it++){
    if (*it == p) return true;
  } return false;
}

void add_pb(const point& p, std::list<point>& plist, std::set<point>& bord){
  if (isinlist(plist,p)) return; // точка уже есть
  plist.push_back(p);
  bord.erase(p);
  for (int i=0; i<point_adjs; i++){
    point p2 = p.adjacent(i);
    if (!isinlist(plist,p2)) bord.insert(p2);
  }
}

void add_pb(const point& p, std::set<point>& pset, std::set<point>& bord){
  if (pset.find(p)!=pset.end()) return; // точка уже есть
  pset.insert(p);
  bord.erase(p);
  for (int i=0; i<point_adjs; i++){
    point p2 = p.adjacent(i);
    if (pset.find(p2)==pset.end()) bord.insert(p2);
  }
}
 
/*
std::set<point> plane(const point& p, int(*f)(int,int) ){
  std::set<point> ret;
  std::queue<point> q;
  short h = f(p.x,p.y);

  q.push(p);
  ret.insert(p);

  while (!q.empty()){
    point p1 = q.front();
    q.pop();
    for (int i=0; i<point_adjs; i++){
      point p2 = p1.adjacent(i);
      if ((f(p2.x, p2.y) == h)&&(ret.insert(p2).second)) q.push(p2);
    }
  }
  return ret;
}
*/
