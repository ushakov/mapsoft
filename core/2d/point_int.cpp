#include "point_int.h"
#include "line_utils.h"

iPoint
adjacent(const iPoint &p, const int dir){
  switch(dir%8){
    case 0: return iPoint(p.x-1,p.y-1);
    case 1: return iPoint(p.x,  p.y-1);
    case 2: return iPoint(p.x+1,p.y-1);
    case 3: return iPoint(p.x+1,p.y  );
    case 4: return iPoint(p.x+1,p.y+1);
    case 5: return iPoint(p.x,  p.y+1);
    case 6: return iPoint(p.x-1,p.y+1);
    case 7: return iPoint(p.x-1,p.y  );
  }
}

int
isadjacent(const iPoint & p1, const iPoint & p2){
  for (int i = 0; i<8; i++){
    if (adjacent(p1, i) == p2) return i;
  }
  return -1;
}


std::set<iPoint>
border(const std::set<iPoint >& pset){
  std::set<iPoint> ret;
  std::set<iPoint>::const_iterator it;
  for (it = pset.begin(); it != pset.end(); it++){
    for (int i=0; i<8; i++){
      iPoint p=adjacent(*it, i);
      if (pset.count(p)==0) ret.insert(p);
    }
  }
  return ret;
}


int
add_pb(const iPoint& p, std::set<iPoint>& pset, std::set<iPoint>& bord){
  if (pset.count(p)) return 0; // точка уже есть
  pset.insert(p);
  bord.erase(p);
  for (int i=0; i<8; i++){
    iPoint p2 = adjacent(p, i);
    if (pset.count(p2)==0) bord.insert(p2);
  }
  return 1;
}

iPoint
my_crn (int k){
  k%=4;
  return iPoint(k/2, (k%3>0)?1:0);
}


dMultiLine
pset2line (const std::set<iPoint>& pset){
  dMultiLine ret, ret1;
  //добавляем все обходы точек
  for (std::set<iPoint>::const_iterator i = pset.begin(); i!=pset.end(); i++){
    for (int k = 0; k<4; k++){
      dLine side;
      dPoint dp(0.5,0.5);
      side.push_back(dPoint(*i+my_crn(k)) - dp);
      side.push_back(dPoint(*i+my_crn(k+1)) - dp);
      // у нас уже может быть такая линия, пройденная в обратном направлении.
      // merge такую пару отловит и удалит, но это долгая процедура.
      // поэтому посмотрим здесь:
      // upd. это не сильно все убыстрило...
      bool ispair=false;
      for (dMultiLine::iterator i = ret.begin(); i!=ret.end(); i++){
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

// see details in https://github.com/slazav/bresenham/blob/master/br.c
std::set<iPoint>
brez(iPoint p1, iPoint p2, const int w, const int sh){

  std::set<iPoint> ret;
  int dx=p2.x-p1.x, dy=p2.y-p1.y;
  int e,j;
  int sx=dx>0;  // line goes right
  int sy=dy>0;  // line goes up

  if (!sx) dx=-dx;
  if (!sy) dy=-dy;

  int s=dx>dy; // line closer to horizontal
  bool sd = (sx&&s) || (!sy&&!s); // negative shift

  // start/stop width
  int w1=0-w + (sd?-sh:sh);
  int w2=1+w + (sd?-sh:sh);

  if (s){
    for (j=w1;j<w2;j++) ret.insert(p1 + iPoint(0,j));
    e = (dy<<1)-dx;
    while (p1.x!=p2.x){
      if (e<0){
        sx?p1.x++:p1.x--; e+=dy<<1;
        for (j=w1;j<w2;j++) ret.insert(p1 + iPoint(0,j));
      }
      else {
        sy?p1.y++:p1.y--; e-=dx<<1;
      }
    }
  }
  else {
    for (j=w1;j<w2;j++) ret.insert(p1 + iPoint(j,0));
    e = (dx<<1)-dy;
    while (p1.y!=p2.y){
      if (e<0){
        sy?p1.y++:p1.y--; e+=dx<<1;
        for (j=w1;j<w2;j++) ret.insert(p1 + iPoint(j,0));
      }
      else {
        sx?p1.x++:p1.x--; e-=dy<<1;
      }
    }
  }
  return ret;
}
