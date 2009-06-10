#include "fig_data.h"

namespace fig {

using namespace std;

void fig_object::set_points(const Line<double> & v){
  clear();
  for (int i=0;i<v.size();i++)
    push_back(Point<int>(int(v[i].x), int(v[i].y)));
}
void fig_object::set_points(const Line<int> & v){
  clear();
  insert(end(), v.begin(), v.end());
}

} //namespace

