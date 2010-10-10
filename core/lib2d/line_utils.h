#ifndef LINE_UTILS_H
#define LINE_UTILS_H

#include "line.h"
#include "point_utils.h"

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

// повернуть линии на угол a вокруг точки p0.
template<typename T>
void lrotate(MultiLine<T> & lines, const T a, const Point<T> & p0 = Point<T>(0,0)){
  double c = cos(a);
  double s = sin(a);
  for (typename MultiLine<T>::iterator l = lines.begin(); l!=lines.end(); l++){
    for (typename Line<T>::iterator p = l->begin(); p!=l->end(); p++){
      double x = p->x-p0.x, y = p->y-p0.y;
      p->x = (T)(x*c - y*s + p0.x);
      p->y = (T)(x*s + y*c + p0.y);
    }
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
// не сместилась от исходного положения более чем на e
template<typename T>
void generalize (MultiLine<T> & lines, double e){
  for (typename MultiLine<T>::iterator l = lines.begin(); l!=lines.end(); l++){

    // какие точки мы хотим исключить:
    std::vector<bool> skip(l->size(),false);

    while (l->size()>2){
      // для каждой точки найдем расстояние от нее до 
      // прямой, соединяющей две соседние (не пропущенные) точки.
      // найдем минимум этой величины
      double min = 1e99; //важно, что >e 
      int mini;
      for (int i=1; i<l->size()-1; i++){
        if (skip[i]) continue;
        int ip, in;
        // помним, что skip[0] и skip[l->size()-1] всегда false
        for (ip=i-1; ip>=0; ip--)          if (!skip[ip]) break;
        for (in=i+1; in<l->size()-1; in++) if (!skip[in]) break;
        Point<T> p1 = (*l)[ip];
        Point<T> p2 = (*l)[i];
        Point<T> p3 = (*l)[in];
        double ll = pdist(p3-p1);
        dPoint v = (p3-p1)/ll;
        double prj = pscal(v, p2-p1);
        double dp;
        if      (prj<=0)  dp = pdist(p2,p1);
        else if (prj>=ll) dp = pdist(p2,p3);
        else              dp = pdist(p2-p1-v*prj);
        // в начале у нас обязатеьно i==1
        if ((i==1) || (min>dp)) {min = dp; mini=i;}
      }
      // если этот минимум меньше e - выкинем точку
      if (min<e) skip[mini]=true;
      else break;
    }

    // сделаем новую линию
    Line<T> newl;
    for (int i = 0; i<l->size(); i++) if (!skip[i]) newl.push_back((*l)[i]);
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

    // Найдем место кратчайшего разреза между ret и очередным куском.
    // Честно пока делать лень, поэтому найдем минимальное расстояние 
    // между вершинами...

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

  for (int j=1; j<line.size(); j++){
    Point<T> p1(line[j-1]);
    Point<T> p2(line[j]);

    double  ll = pdist(p1,p2);
    if (ll==0) continue;
    dPoint v = dPoint(p2-p1)/ll;

    double ls = pdist(pt,p1);
    double le = pdist(pt,p2);

    if (ls<maxdist){ maxdist=ls; pt=p1; vec=v; }
    if (le<maxdist){ maxdist=le; pt=p2; vec=v; }

    double prl = pscal(dPoint(pt-p1), v);

    if ((prl>=0)&&(prl<=ll)) { // проекция попала на отрезок
      Point<T> pc = p1 + Point<T>(v * prl);
      double lc=pdist(pt,pc);
      if (lc<maxdist) { maxdist=lc; pt=pc; vec=v; }
    }
  }
  return maxdist;
}

template<typename T>
double nearest_pt(const MultiLine<T> & lines, dPoint & vec, Point<T> & pt, double maxdist){
  for (typename MultiLine<T>::const_iterator i  = lines.begin(); i != lines.end(); i++){
    maxdist = nearest_pt(*i, vec, pt, maxdist);
  }
  return maxdist;
}


#endif
