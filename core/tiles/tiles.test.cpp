#include <cassert>
#include <iostream>
#include "tiles.h"
#include "opts/opts.h"
int
main(){
  try{


    // avoid zero lat or lon because it is some small number,
    // different on i586 and x86_64
    Tiles calc;
std::cerr << "t-(0,0,0): " << calc.tile_to_range(0,0,0) << "\n";
std::cerr << "t-(0,0,1): " << calc.tile_to_range(0,0,1) << "\n";
std::cerr << "t-(1,0,2): " << calc.tile_to_range(1,0,2) << "\n";
std::cerr << "g-(0,0,0): " << calc.gtile_to_range(0,0,0) << "\n";
std::cerr << "g-(0,1,1): " << calc.gtile_to_range(0,1,1) << "\n";
std::cerr << "g-(1,0,2): " << calc.gtile_to_range(1,0,2) << "\n";

    assert(type_to_str(calc.tile_to_range(0,0,0)) == "360x170.102258-180-85.0511288");
    assert(type_to_str(calc.tile_to_range(0,0,1)) == "180x85.0511288-180-85.0511288");
    assert(type_to_str(calc.tile_to_range(1,0,2)) == "90x18.5378683-90-85.0511288");

    assert(type_to_str(calc.gtile_to_range(0,1,1)) == "180x85.0511288-180-85.0511288");
    assert(type_to_str(calc.gtile_to_range(1,0,2)) == "90x18.5378683-90+66.5132604");

    assert(calc.pt_to_tile(dPoint(1,1),0) == iPoint(0,0));
    assert(calc.pt_to_tile(dPoint(1,1),1) == iPoint(1,1));
    assert(calc.pt_to_tile(dPoint(1,1),2) == iPoint(2,2));

    assert(calc.pt_to_gtile(dPoint(1,1),0) == iPoint(0,0));
    assert(calc.pt_to_gtile(dPoint(1,1),1) == iPoint(1,0));
    assert(calc.pt_to_gtile(dPoint(1,1),2) == iPoint(2,1));

    assert(calc.range_to_tiles(dRect(-1,-1,2,2),0) == iRect(0,0,1,1));
    assert(calc.range_to_tiles(dRect(-1,-1,2,2),1) == iRect(0,0,2,2));
    assert(calc.range_to_tiles(dRect(-1,-1,2,2),2) == iRect(1,1,2,2));

    assert(calc.range_to_gtiles(dRect(-1,-1,2,2),0) == iRect(0,0,1,1));
    assert(calc.range_to_gtiles(dRect(-1,-1,2,2),1) == iRect(0,0,2,2));
    assert(calc.range_to_gtiles(dRect(-1,-1,2,2),2) == iRect(1,1,2,2));


    assert(calc.pt_to_gtile(dPoint(77.695619,32.183764),15) == iPoint(23456,13287));
  }
  catch (Err e){
    std::cerr << e.get_error() << "\n";
  }
  return 0;
}
