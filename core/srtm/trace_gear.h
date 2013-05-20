#include <set>
#include <2d/point_int.h>
#include <srtm/srtm3.h>

class trace_gear{
public:
  srtm3 & S;
  std::set<iPoint> P, B; // множество пройденных точек, его граница
  int n,ns;              // счетчик всех точек, счетчик бессточных территорий
  iPoint p0,pp;          // исходная точка, точка последнего шага
  int h0,hp;             // исходная высота, высота последнего шага

  trace_gear(srtm3 & S_, const iPoint & p0_):
      S(S_), p0(p0_), pp(p0_), n(0), ns(0){
    h0 = hp = S.geth(p0);
    P.insert(p0);
    for (int i=0; i<8; i++) B.insert(adjacent(p0,i));
  }

  iPoint go(bool down){
    // найдем минимум на границе и добавим его в список
    iPoint p=*B.begin();
    int h = S.geth(p, true);
    std::set<iPoint>::iterator b;
    for (b = B.begin(); b!=B.end(); b++){
      int h1 = S.geth(*b, true);
      if (h1 < srtm_min) continue;
      if ((!down && h < h1) || ( down && h > h1)) { h=h1; p=*b;}
    }

    // добавим точку, пересчитаем границу
    P.insert(p);
    B.erase(p);
    for (int i=0; i<8; i++){
      iPoint p2 = adjacent(p, i);
      if (P.count(p2)==0) B.insert(p2);
    }

    n++; ns++;
    // если мы нашли более низкую точку, чем прошлая - делаем шаг,
    // обнуляем счетчик бессточных территорий
    if ((!down && h > hp) || (down && h < hp)) { hp=h; ns=0; pp=p; }
    return p;
  }
};
