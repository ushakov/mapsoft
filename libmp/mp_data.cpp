#include "mp_data.h"

namespace mp {

using namespace std;

void mp_object::set_points(const Line<double> & v){
  clear();
  insert(begin(), v.begin(), v.end());
}

} //namespace
