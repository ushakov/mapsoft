#include <iostream>
#include "point_cross.h"

// Пересечение двух отрезков
// Отрезок включает первую точку, но не включает последнюю
// Если отрезки не пересекаются - возникает исключение


int main(){
  int n=0, np=0;
  iPoint P;

  try{

  n++;
  P=find_cross(iPoint(0,0), Point<int>(6,8), Point<int>(6,0), Point<int>(0,8));
  if (P == iPoint(3,4)) np++;
  else std::cerr << "find_cross error (" << n << "): " << P << "\n";

  n++;
  P=find_cross(iPoint(1,1), Point<int>(5,3), Point<int>(2,4), Point<int>(4,0));
  if (P == iPoint(3,2)) np++;
  else std::cerr << "find_cross error (" << n << "): " << P << "\n";

  n++;
  try{
    P=find_cross(iPoint(1,1), Point<int>(4,3), Point<int>(2,4), Point<int>(2,2));
    std::cerr << "find_cross error (" << n << ")\n";
  }
  catch(int i){
    np++;
  }

  n++;
  try{
    P=find_cross(iPoint(1,1), Point<int>(4,3), Point<int>(2,4), Point<int>(4,4));
    std::cerr << "find_cross error (" << n << ")\n";
  }
  catch(int i){
    np++;
  }

  n++;
  try{
    P=find_cross(iPoint(1,1), Point<int>(4,3), Point<int>(7,5), Point<int>(10,7));
    std::cerr << "find_cross error (" << n << ")\n";
  }
  catch(int i){
    np++;
  }

/*  n++;
  P=find_cross(iPoint(1,1), Point<int>(3,3), Point<int>(2,2), Point<int>(4,4));
  std::cerr << "find_cross : " << P << "\n";
  if (P == iPoint(3,2)) np++;
  else std::cerr << "find_cross (" << n << "): " << P << "\n";

  n++;
  P=find_cross(iPoint(1,1), Point<int>(3,3), Point<int>(3,3), Point<int>(4,4));
  if (P == iPoint(3,2)) np++;
  else std::cerr << "find_cross (" << n << "): " << P << "\n";

  n++;
  P=find_cross(iPoint(1,1), Point<int>(3,3), Point<int>(1,1), Point<int>(4,4));
  if (P == iPoint(3,2)) np++;
  else std::cerr << "find_cross (" << n << "): " << P << "\n";

  n++;
  P=find_cross(iPoint(1,1), Point<int>(3,3), Point<int>(1,1), Point<int>(4,5));
  if (P == iPoint(3,2)) np++;
  else std::cerr << "find_cross (" << n << "): " << P << "\n";*/

  double d;

  n++;
  d=find_dist(iPoint(-1,7), Point<int>(0,0), Point<int>(6,8));
  if (d == 5) np++;
  else std::cerr << "find_dist error (" << n << "): " << d << "\n";

  n++;
  d=find_dist(iPoint(-3,0), Point<int>(6,8), Point<int>(0,0));
  if (d == 3) np++;
  else std::cerr << "find_dist error (" << n << "): " << d << "\n";

  n++;
  d=find_dist(iPoint(12,16), Point<int>(6,8), Point<int>(0,0));
  if (d == 10) np++;
  else std::cerr << "find_dist error (" << n << "): " << d << "\n";


  }
  catch(int j){
  }

  std::cerr << "Result: " << np << "/" << n << "\n";

}
/*
// Кратчайшее расстояние между точкой и отрезком
template <typename T>
double find_dist(const Point<T> & p,
                 const Point<T> & p1, const Point<T> & p2){

  double  ll = pdist(p1,p2);
  if (ll==0) return pdist(p,p1); // отрезок нулевой длины

  dPoint vec = (p2-p1)/ll;

  double l1 = pdist(p,p1);
  double l2 = pdist(p,p2);
  double ret = l1<l2 ? l1:l2;

  double prl = pscal(p-p1, vec);

  if ((prl>=0)&&(prl<=ll)){ // проекция попала на отрезок
    dPoint pc = p1 + vec * prl;
    double lc=pdist(p,pc);
    ret= ret<lc ? ret:lc;
  }
  return ret;
}

// Кратчайшее расстояние между двумя отрезками
template <typename T>
double find_dist(const Point<T> & p1, const Point<T> & p2,
                 const Point<T> & p3, const Point<T> & p4){
  return min( min(find_dist(p1,p3,p4), find_dist(p2,p3,p4)),
              min(find_dist(p3,p1,p2), find_dist(p4,p1,p2)));
}

*/
