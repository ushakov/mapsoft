#include "fig_data.h"

namespace fig {

using namespace std;

void fig_object::set_points(const dLine & v){
  clear();
  for (int i=0;i<v.size();i++)
    push_back(iPoint(int(v[i].x), int(v[i].y)));
}
void fig_object::set_points(const iLine & v){
  clear();
  insert(end(), v.begin(), v.end());
}

} //namespace

