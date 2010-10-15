#include "line_dist.h"

LineDist::LineDist(const dLine & _line): line(_line){
  current_l=0;
  current_n=0;

  double l=0;
  ls.push_back(0);
  for (int j=1; j<_line.size(); j++){
    dPoint p1 (_line[j-1]);
    dPoint p2 (_line[j]);
    if (p1 == p2) continue;
    l+=pdist(p1,p2); 
    ls.push_back(l);
  }
}

double
LineDist::length() const {
  return ls[ls.size()-1];
}


dPoint
LineDist::pt() const{
  if (is_end()) return line[current_n];
  else return (line[current_n] + (line[current_n+1]-line[current_n]) *  
               (current_l-ls[current_n])/(ls[current_n+1]-ls[current_n]) );
}

double
LineDist::dist() const {
  return current_l;
}

dPoint
LineDist::tang() const{
  if (is_end()) return dPoint(1,0);
  return pnorm(line[current_n+1]-line[current_n]);
}

dPoint
LineDist::norm() const{
  dPoint ret = tang();
  return dPoint(-ret.y, ret.x);
}

dLine
LineDist::get_points(double dl){
  dLine ret;
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
  current_l = l;
  *ret.rbegin() = pt();
  return ret;
}

void
LineDist::move_begin(){
  current_l = 0;
  current_n = 0;
}

void
LineDist::move_end(){
  current_n = ls.size()-1;
  current_l = ls[current_n];
}

void
LineDist:: move_frw(double dl){
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

void
LineDist::move_bck(double dl){
  if (dl < 0) {move_frw(-dl); return;}
  if (dl == 0) return;
  double l = current_l - dl;
  while (current_l > l){
    if (current_n == 0) return;
    move_bck_to_node();
  }
  current_l=l;
}

void
LineDist::move_frw_to_node(){
  if (current_n == ls.size()-1) return;
  current_n++;
  current_l=ls[current_n];
}

void
LineDist::move_bck_to_node(){
  if ((current_l==ls[current_n]) && (current_n!=0))
    current_n--;
  current_l=ls[current_n];
}

bool
LineDist::is_end() const{
  return (current_n == ls.size()-1);
}

bool
LineDist::is_begin() const{
  return (current_n == 0);
}

