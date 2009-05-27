#ifndef LINE_DIST_H
#define LINE_DIST_H

// линия параметризуется длиной
// можно получать точки через некоторые интервалы
// сделано для рисования условных обозначений - всяких сложных пунктиров,
// газопроводов, лэп...

#include "../../core/lib2d/line.h"
#include "../../core/lib2d/point_utils.h"
#include <vector>

template <typename T>
class LineDist{

  Line<double>   line; // копия линии

  std::vector<double> ls; // длина ломаной до точки [i]
  double  current_l; // 
  int     current_n; // 

  public:
    LineDist(const Line<T> & _line){
      line.insert(line.end(), _line.begin(), _line.end());
      current_l=0;
      current_n=0;

      double l=0;
      ls.push_back(0);
      for (int j=1; j<_line.size(); j++){
        Point<double> p1 (_line[j-1]);
        Point<double> p2 (_line[j]);
        if (p1 == p2) continue;
        l+=pdist(p1,p2); 
        ls.push_back(l);
      }
    }


  // длина всей линии
  double length() const {return ls[ls.size()-1];}

  // текущее расстояние от начала
  double dist() const {return current_l;}

  // текущая точка 
  Point<double> pt() const{
    if (is_end()) return line[current_n];
    else return (line[current_n] + (line[current_n+1]-line[current_n]) *  
                 (current_l-ls[current_n])/(ls[current_n+1]-ls[current_n]) );
  }
  // единичный касаельный вектор 
  Point<double> tang() const{
    if (is_end()) return Point<double>(1,0);
    return pnorm(line[current_n+1]-line[current_n]);
  }
  // единичный перпендикулярный вектор 
  Point<double> norm() const{
    Point<double> ret = tang();
    return Point<double>(-ret.y, ret.x);
  }

  // вырезать из ломеной кусок от текущей точки, длиной dl>0
  // сдвинуть текущую точку
  Line<T>  get_points(double dl){
    Line<T> ret;
    if (dl <= 0) return ret;

    double l = current_l + dl;

    // добавим первую точку
    ret.push_back(pt());
    if (is_end()) return ret;

    while (current_l < l){
      if (is_end()) return ret;
      move_frw_to_node();
      // добавим следующий узел
      ret.push_back(line[current_n]);
    }
    // последний узел находится дальше, чем нужная нам длина.
    // подвинем...
    current_n--;
    current_l=l;
    *ret.rbegin() = pt();
    return ret;
  }

  // сдвинуть текущую точку в начало
  void move_begin(){current_l = 0; current_n=0;}

  // подвинуть текущую точку вперед на dl
  void move_frw(double dl){

    if (dl < 0) {move_bck(-dl); return;}
    if (dl == 0) return;
    double l = current_l + dl;
    while (current_l < l){
      if (is_end()) return;
      move_frw_to_node();
    }
    current_n--;
    current_l=l;
  }
  // подвинуть текущую точку назад на dl
  void move_bck(double dl){
    if (dl < 0) {move_frw(-dl); return;}
    if (dl == 0) return;
    double l = current_l - dl;
    while (current_l > l){
      if (current_n == 0) return;
      move_bck_to_node();
    }
    current_l=l;    
  }
  // подвинуть текущую точку вперед до ближайшего узла
  void move_frw_to_node(){
    if (current_n == ls.size()-1) return;
    current_n++;
    current_l=ls[current_n];
  }
  // подвинуть текущую точку назад до ближайшего узла
  void move_bck_to_node(){
    if ((current_l==ls[current_n]) && (current_n!=0)) current_n--;
    current_l=ls[current_n];
  }
  bool is_end() const{
    return (current_n == ls.size()-1);
  }

};

#endif
