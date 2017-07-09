#include <cassert>
#include <iostream>
#include "tiles.h"
#include "opts/opts.h"

main(){
  try{

    Tiles calc;
    assert(type_to_str(calc.tile_to_range(iPoint(0,0),0)) == "360x170.102-180-85.0511");
    assert(type_to_str(calc.tile_to_range(iPoint(0,0),1)) == "180x85.0511-180-85.0511");
    assert(type_to_str(calc.tile_to_range(iPoint(1,1),1)) == "180x85.0511+0+0");
    assert(calc.pt_to_gtile(dPoint(77.695619,32.183764),15) == iPoint(23456,13287));
  }
  catch (Err e){
    std::cerr << e.get_error() << "\n";
  }
}
