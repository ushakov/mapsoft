#ifndef LINE_POLYCROP_H
#define LINE_POLYCROP_H

#include "line.h"
#include "point_utils.h"
#include "point_cross.h"

/// обрезание линий по многоугольнику. 
/// для резки многоугольников работает не всегда, треьует большого переписывания :(

// лежит ли точка в многоугольнике poly
template<typename T>
bool test_pt (const dPoint & pt, const T & poly){
  double a = 0;
  typename T::const_iterator p1,p2;
  for (int i = 0; i<poly.size(); i++){
    dPoint v1 = Point<double>(poly[i]) - pt;
    dPoint v2 = Point<double>(poly[(i+1)%poly.size()]) - pt;
    double dd = pdist(v1)*pdist(v2);
    if (dd==0) return true;

    double s = v1.x*v2.y - v1.y*v2.x;
    double c = pscal(v1,v2)/dd;
    if (fabs(c)>=1) {continue;}

    if (s<0) a+=acos(c); else a-=acos(c);
  }

  return (fabs(a)>M_PI);
}

// обрезать все линии, входящие/не входящие в многоугольник cutter и добавить их в lines1
template<typename T>
void crop_lines(std::vector<T> & lines,
                std::vector<T> & lines1,
                const T & cutter, bool cutouter){
  for (int j = 0; j<cutter.size(); j++){
    for (typename std::vector<T>::iterator l = lines.begin(); l!=lines.end(); l++){
      for (int i = 0; i<l->size()-1; i++){
        dPoint pt;
        try { pt = find_cross((*l)[i], (*l)[i+1], cutter[j], cutter[(j+1)%cutter.size()]); }
        catch (int n) {continue;}
        // разбиваем линию на две, уже обработанный кусок помещаем перед l
        T l1(*l); l1.clear(); // мы хотим работать не только с линиями, но и с потомками!
        for (int k=0; k<=i; k++) l1.push_back((*l)[k]);
        l1.push_back(pt);
        l=lines.insert(l, l1); l++;
        // из *l стираем все точки до i-1-й
        l->erase(l->begin(), l->begin()+i);
        *(l->begin()) = pt;
        // продолжаем со второго звена оставшейся линии
        i=1;
      }
    }
  }
  // теперь переместим те линии, которые не попадают в нужный район
  typename std::vector<T>::iterator l=lines.begin();
  while (l!=lines.end()){
    if (l->size()==0) {l=lines.erase(l); continue;}
    // посчитаем число точек внутри - число точек вне.
    int sum=0;
    for (int i = 1; i<l->size(); i++){
      sum+= (cutouter xor test_pt(dPoint((*l)[i]+(*l)[i-1])/2.0, cutter))? -1:1;
    }
    if (sum<0) {lines1.push_back(*l);  l=lines.erase(l); continue;}
    l++;
  }
}

#endif /* LINE_POLYCROP_H */
