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

void fig_world::remove_empty_comp(){
  int removed;
  do{
    removed=0;
    iterator o=begin();
    while (o!=end()){
      if (o->type==6){
        iterator on=o; on++;
        if ((on!=end()) && (on->type==-6)){
          o=erase(o);
          o=erase(o);
          removed++;
          continue;
        }
      }
      o++;
    }
  }
  while(removed>0);
}

} //namespace

