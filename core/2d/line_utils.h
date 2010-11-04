#ifndef LINE_UTILS_H
#define LINE_UTILS_H

#include "line.h"

///\addtogroup lib2d
///@{
///\defgroup line_utils
///@{

//преобразовать прямоугольник в линию из 4 точек
template<typename T>
Line<T> rect2line(const Rect<T> & r){
  Line<T> ret;
  ret.push_back(r.TLC());
  ret.push_back(r.TRC());
  ret.push_back(r.BRC());
  ret.push_back(r.BLC());
  ret.push_back(r.TLC());
  return  ret;
}

// повернуть линию на угол a вокруг точки p0.
template<typename T>
void lrotate(Line<T> & line, const T a, const Point<T> & p0 = Point<T>(0,0)){
  double c = cos(a);
  double s = sin(a);
  for (typename Line<T>::iterator p = line.begin(); p!=line.end(); p++){
    double x = p->x-p0.x, y = p->y-p0.y;
    p->x = (T)(x*c - y*s + p0.x);
    p->y = (T)(x*s + y*c + p0.y);
  }
}

// повернуть линии на угол a вокруг точки p0.
template<typename T>
void lrotate(MultiLine<T> & lines, const T a, const Point<T> & p0 = Point<T>(0,0)){
  for (typename MultiLine<T>::iterator l = lines.begin(); l!=lines.end(); l++){
    lrotate(*l, a, p0);
  }
}

// склеивание линий, если их концы ближе e
template<typename T>
void merge (MultiLine<T> & lines, T e){

  //убираем вообще двойные линии
  for (typename MultiLine<T>::iterator i1 = lines.begin(); i1!=lines.end(); i1++){
    for (typename MultiLine<T>::iterator i2 = i1; i2!=lines.end(); i2++){
      if (i1==i2) continue;
      if ((*i1==*i2) || (i1->isinv(*i2))){
        lines.erase(i2);
        i1 = lines.erase(i1); i1--;
        i2 = i1;
      }
    }
  }
  for (typename MultiLine<T>::iterator i1 = lines.begin(); i1!=lines.end(); i1++){
    for (typename MultiLine<T>::iterator i2 = i1; i2!=lines.end(); i2++){
      if (i1==i2) continue;
      Line<T> tmp;
      if      (pdist(*(i1->begin()),*(i2->begin()))<e)   {tmp.insert(tmp.end(), i1->rbegin(), i1->rend()); tmp.insert(tmp.end(), i2->begin()+1, i2->end());}
      else if (pdist(*(i1->begin()),*(i2->rbegin()))<e)  {tmp.insert(tmp.end(), i1->rbegin(), i1->rend()); tmp.insert(tmp.end(), i2->rbegin()+1, i2->rend());}
      else if (pdist(*(i1->rbegin()),*(i2->begin()))<e)  {tmp.insert(tmp.end(), i1->begin(), i1->end()); tmp.insert(tmp.end(), i2->begin()+1, i2->end());}
      else if (pdist(*(i1->rbegin()),*(i2->rbegin()))<e) {tmp.insert(tmp.end(), i1->begin(), i1->end()); tmp.insert(tmp.end(), i2->rbegin()+1, i2->rend());}
      else continue;
      i1->swap(tmp);
      lines.erase(i2); 
      i2=i1;
    }
  }
}

// разбиение линии на несколько, каждая не более points точек
template<typename T>
void split (MultiLine<T> & lines, int points){

  if (points < 2) return;
  for (typename MultiLine<T>::iterator i = lines.begin(); i!=lines.end(); i++){
    while (i->size() > points){
      // points последних точек
      Line<T> newline;
      newline.insert(newline.begin(), i->rbegin(), i->rbegin()+points);
      lines.push_back(newline);
      i->resize(i->size()-points+1);
    }
  }
}

// Убрать из линии некоторые точки, так, чтобы линия
// не сместилась от исходного положения более чем на e (если e>0)
// или уменьшить число точек в линии до np (если np>2)
// todo: closed lines
template<typename T>
Line<T> generalize(const Line<T> & line, double e, int np){
  // какие точки мы хотим исключить:
  std::vector<bool> skip(line.size(), false);

  np-=2; // end points are not counted
  while (1){
    // для каждой точки найдем расстояние от нее до
    // прямой, соединяющей две соседние (не пропущенные) точки.
    // найдем минимум этой величины
    double min=-1;
    int mini; // index of point with minimal deviation
    int n=0;
    for (int i=1; i<int(line.size())-1; i++){
      if (skip[i]) continue;
      n++; // count point we doesn't plan to skip
      int ip, in; // previous and next indexes
      // skip[0] and skip[line.size()-1] are always false
      for (ip=i-1; ip>=0; ip--)            if (!skip[ip]) break;
      for (in=i+1; in<line.size()-1; in++) if (!skip[in]) break;
      Point<T> p1 = line[ip];
      Point<T> p2 = line[i];
      Point<T> p3 = line[in];
      double ll = pdist(p3-p1);
      dPoint v = (p3-p1)/ll;
      double prj = pscal(v, p2-p1);
      double dp;
      if      (prj<=0)  dp = pdist(p2,p1);
      else if (prj>=ll) dp = pdist(p2,p3);
      else              dp = pdist(p2-p1-v*prj);
      if ((min<0) || (min>dp)) {min = dp; mini=i;}
    }
    if (n<=2) break;
    // если этот минимум меньше e или точек в линии больше np - выкинем точку
    if ( ((e>0) && (min<e)) ||
         ((np>0) && (n>np))) skip[mini]=true;
    else break;
  }
  // сделаем новую линию
  Line<T> ret;
  for (int i = 0; i<line.size(); i++) if (!skip[i]) ret.push_back(line[i]);
  return ret;
}

// То же для набора линий. Слишком короткие куски исчезают.
template<typename T>
void generalize(MultiLine<T> & lines, double e){
  for (typename MultiLine<T>::iterator l = lines.begin(); l!=lines.end(); l++){
    Line<T> newl = generalize(*l, e, -1);
    l->swap(newl);
    // если осталась линия из двух близких точек - сотрем ее.
    if ((l->size() == 2) && (pdist((*l)[0],(*l)[1]) < e)){
      l=lines.erase(l); l--;
    }
  }
}

// FIG does not support polygons with multiple segments while
// MP, OCAD, PS does. This function converts multiple segments into one.
template<typename T>
Line<T> join_polygons(const MultiLine<T> & L){

  Line<T> ret;

  typename MultiLine<T>::const_iterator l = L.begin();
  ret = *l; l++;
  while (l!=L.end()){

    // Найдем место кратчайшего разреза между вершиной ret и
    // вершиной очередного куска.

    double dist = 1e99;

    typename Line<T>::iterator  i1,q1 ;
    typename Line<T>::const_iterator  i2,q2;
      // i1,i2 -- пара вершин
      // q1,q2 -- искомое

    for (i1=ret.begin(); i1!=ret.end(); i1++){
      for (i2=l->begin(); i2!=l->end(); i2++){

        double d = pdist(*i1, *i2);
        if (d < dist){
          dist = d;
          q1=i1; q2=i2;
        }
      }
    }

    // вставим кусок в разрез
    Line<T> tmp;
    tmp.push_back(*q1);
    tmp.insert(tmp.end(), q2, l->end());
    tmp.insert(tmp.end(), l->begin(), q2);
    tmp.push_back(*q2);
    ret.insert(q1, tmp.begin(), tmp.end());

    l++;
  }
  return ret;
}

// Найти ближайшую к pt точку из линии line.
// Если найдена точка ближе, чем maxdist, то в vec записывается
// единичный вектор направления линии, а в pt - ближайшая точка.
// Если точки ближе maxdist не найдено - возвращается maxdist,
// а vec и pt не меняются.
template<typename T>
double nearest_pt(const Line<T> & line, dPoint & vec, Point<T> & pt, double maxdist){

  Point<T> pm = pt;

  for (int j=1; j<line.size(); j++){
    Point<T> p1(line[j-1]);
    Point<T> p2(line[j]);

    double  ll = pdist(p1,p2);
    if (ll==0) continue;
    dPoint v = dPoint(p2-p1)/ll;

    double ls = pdist(pt,p1);
    double le = pdist(pt,p2);

    if (ls<maxdist){ maxdist=ls; pm=p1; vec=v; }
    if (le<maxdist){ maxdist=le; pm=p2; vec=v; }

    double prl = pscal(dPoint(pt-p1), v);

    if ((prl>=0)&&(prl<=ll)) { // проекция попала на отрезок
      Point<T> pc = p1 + Point<T>(v * prl);
      double lc=pdist(pt,pc);
      if (lc<maxdist) { maxdist=lc; pm=pc; vec=v; }
    }
  }
  pt=pm;
  return maxdist;
}

template<typename T>
double nearest_pt(const MultiLine<T> & lines, dPoint & vec, Point<T> & pt, double maxdist){
  dPoint pm=pt;
  for (typename MultiLine<T>::const_iterator i  = lines.begin(); i != lines.end(); i++){
    Point<T> p = pt;
    maxdist = nearest_pt(*i, vec, p, maxdist);
    if ( p != pt) { pm = p;}
  }
  pt=pm;
  return maxdist;
}

/// Found bounding convex polygon for points.
/// Resulting polygon start from point with minimal x,
/// it is always clockwise-oriented, its last point =
/// first point
template <typename T>
Line<T> convex_border(const Line<T> & points){
  Line<T> ret;
  if (points.size()==0) return ret;
  typename Line<T>::const_iterator p, p0, p1;

  // find point with minimal x
  p0=points.begin();
  for (p=points.begin(); p!=points.end(); p++){
    if (p0->x > p->x) p0=p;
  }

  dPoint v0(0,1);

  do {
    ret.push_back(*p0);
    p1=p0;
    p1++; if (p1==points.end()) p1=points.begin();
    if (p0==p1) break;

    // find point with maximal v*norm(p-p0)
    double cmax=-1;
    for (p=points.begin(); p!=points.end(); p++){
      if (p == p0) continue;
      dPoint v=pnorm(*p - *p0);
      double c=pscal(v0, v);
      if (cmax < c) { cmax=c; p1=p;}
    }
    v0=pnorm(*p1 - *p0);
    p0=p1;
    assert (ret.size() <= points.size());
  } while (*p1!=*ret.begin());
  return ret;
}

#endif
