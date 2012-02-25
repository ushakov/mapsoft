#include "poly_tester.h"
//#include "2d/line_utils.h"

using namespace std;

poly_tester::poly_tester(const dLine & brd) : border(brd){
  sides.clear();
  int n = border.size();
  for (int i = 0; i < n; i++){
    side S;
    S.x1 = int(border[i%n].x);
    S.y1 = int(border[i%n].y);
    S.x2 = int(border[(i+1)%n].x);
    S.y2 = int(border[(i+1)%n].y);
    if (S.y1==S.y2) continue; // no need for horisontal sides
    S.k = double(S.x2-S.x1)/double(S.y2-S.y1); // side slope
    sides.push_back(S);
  }
}

bool
poly_tester::test(const iPoint & p) const{
  int k=0; // number of sides crossed by the ray (x,y) - (inf,y)
  int e = sides.size();
  for (int i = 0; i < e; ++i){
    side const & S = sides[i];
    if ((S.y1 > p.y)&&(S.y2 > p.y)) continue; // side is above the ray
    if ((S.y1 < p.y)&&(S.y2 < p.y)) continue; // side is below the ray
    if ((S.x2 < p.x)&&(S.x1 < p.x)) continue; // side is on the left of the ray
    int x0 = int(S.k * double(p.y - S.y1)) + S.x1; // crossing point
    if (x0 < p.x) continue; // crossing point is on the left of the ray

    // тут есть проблемы во-первых со стыками сторон, которые учитываются дважды,
    // а во вторых с нижней точкой v-образной границы, которая должна учитываться дважды.
    // решение такое: у сторон, идущих вниз не учитываем первую точку, 
    // а у сторон, идущих вверх - последнюю! 
    if (((S.y2<p.y)&&(S.y1==p.y)) ||
        ((S.y1<p.y)&&(S.y2==p.y))) continue;
    k++;
  }
  return k%2==1;
}

int // The same code!
poly_tester::nearest_border (const iPoint &p) const {
  int dist=0xFFFFFF;
  int k=0; // number of sides crossed by the ray (x,y) - (inf,y)
  int e = sides.size();
  for (int i = 0; i < e; ++i){
    side const & S = sides[i];
    if ((S.y1 > p.y)&&(S.y2 > p.y)) continue; // side is above the ray
    if ((S.y1 < p.y)&&(S.y2 < p.y)) continue; // side is below the ray
    if ((S.x2 < p.x)&&(S.x1 < p.x)) continue; // side is on the left of the ray
    int x0 = int(S.k * double(p.y - S.y1)) + S.x1; // crossing point
    if (x0 < p.x) continue; // crossing point is on the left of the ray
    if (((S.y2<p.y)&&(S.y1==p.y)) ||
      ((S.y1<p.y)&&(S.y2==p.y))) continue;
    k++;
    if (dist > x0 - p.x) dist = x0 - p.x;
  }
  return k%2==1 ? dist:-dist;
}

bool // Not accurate. TODO: use rect_crop
poly_tester::test(const iRect & r) const{
  int lx = 0; int ly=0;
  int rx = 0; int ry=0;
  iPoint p1 = r.TLC();
  iPoint p2 = r.BRC();
  dLine::const_iterator p;
  for (p = border.begin(); p !=border.end(); p++){
    if (p->x < p1.x) lx++;
    if (p->x > p2.x) rx++;
    if (p->y < p1.y) ly++;
    if (p->y > p2.y) ry++;
  }
  int s = border.size();
  return !((lx == s) ||
           (ly == s) ||
           (rx == s) ||
           (ry == s));
}
