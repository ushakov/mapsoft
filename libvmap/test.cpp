#include "vmap.h"

main(){
  vmap::world w;
  vmap::read_dir("./m1", w);
  vmap::write_dir("./m2", w);
}