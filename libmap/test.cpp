#include "vmap.h"

main(){
  map::world w;
  map::read_dir("./m1", w);
  map::write_dir("./m2", w);
}