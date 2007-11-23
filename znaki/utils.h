#ifndef FIG_UTILS_H
#define FIG_UTILS_H

#include <vector>
#include <string>
#include <sstream>
#include "../geo_io/fig.h"

// заключить fig-объекты в составной объект. Комментарий
// составного объекта копируется из первого объекта (!)

void fig_make_comp(std::list<fig::fig_object> & objects){
  if ((objects.size()<1) || (objects.begin()->size()<1)) return;

  int minx=(*objects.begin())[0].x;
  int maxx=(*objects.begin())[0].x;
  int miny=(*objects.begin())[0].y;
  int maxy=(*objects.begin())[0].y;
  for (std::list<fig::fig_object>::const_iterator 
       i = objects.begin(); i != objects.end(); i++){
     if (i->type == 1){
       int rx = i->radius_x;
       int ry = i->radius_y;
       int cx = i->center_x;
       int cy = i->center_y;
       if (minx > cx-rx) minx = cx-rx;
       if (maxx < cx+rx) maxx = cx+rx;
       if (miny > cy-ry) miny = cy-ry;
       if (maxy < cy+ry) maxy = cy+ry;
     } else {
       for (int j = 0; j < i->size(); j++){
         int x = (*i)[j].x;
         int y = (*i)[j].y;
         if (minx > x) minx = x;
         if (maxx < x) maxx = x;
         if (miny > y) miny = y;
         if (maxy < y) maxy = y;
      }
    }
  }
  fig::fig_object o = *objects.begin();
  o.type=6;
  o.clear();
  o.push_back(Point<int>(minx,miny));
  o.push_back(Point<int>(maxx,maxy));
  objects.insert(objects.begin(), o);
  o.type = -6; o.comment.clear();
  objects.insert(objects.end(), o);
}

// ключ, зависящий от координат объекта
template <typename T>
std::string make_key(const std::vector<Point<T> > & o){
  std::ostringstream s;
    s << "KEY: ";
  for (int i = 0; i < o.size(); i++){
    s << o[i].x << " " << o[i].y << "  ";
  }
  return s.str();
}

// сравнение ключей -- максимальное расхождение в координатах должно быть < min
bool key_cmp(const std::string &k1, const std::string &k2, double min){
  std::istringstream s1(k1), s2(k2);
  if ((k1.compare(0,4, "KEY:")!=0) || (k2.compare(0,4, "KEY:")!=0)) return false;
//std::cerr << "KEY COMP:\n";
//std::cerr << k1 << "\n" << k2 << "\n";
  s1.seekg(4); s2.seekg(4);
  while (!s1.eof() && !s2.eof()){
    double n1, n2;
    s1 >> n1; s2 >> n2;
    if (fabs(n1-n2)>min) return false;
  }
  if (!s1.eof() || !s2.eof()) return false;
//std::cerr << "OK\n";
  return true;
}

// размещение подписи. В идеале должно быть умным, но пока сделано по-простому
void fig_make_label(std::list<fig::fig_object> & W, const std::string & text, const std::string & mask){
    if ((W.size()==0) || (W.begin()->size()==0) || (text.size()==0)) return;

    fig::fig_object o1 = fig::make_object(mask);
    o1.text = text;
    o1.comment.push_back(make_key(*W.begin()));

    // ищем правую верхнюю точку
    Point<int> p = (*W.begin())[0];
    for (int i = 0; i<W.begin()->size(); i++){
      Point<int> p1 = (*W.begin())[i];
      if (p1.x-p1.y > p.x-p.y) p=p1;
    }
    int dx = int(0.1*fig::cm2fig);
    p += Point<int>(dx, -dx);
    o1.push_back(p);
    W.push_back(o1);
}

#endif
