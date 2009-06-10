#include <iomanip>
#include "../core/libgeo/geo_convs.h"
#include "../core/libgeo/geo_nom.h"

int main(int argc, char **argv){
  if (argc<=1) std::cout << "lat: 0 0 lon: 0 0";
  else { 
    Rect<double> brd = convs::nom_range(argv[1]);
    Point<double> p1 = brd.TLC(), p2 = brd.TRC(), p3 = brd.BRC(), p4 = brd.BLC();
    convs::pt2pt c(Datum("pulkovo"), Proj("lonlat"), Options(), Datum("wgs84"), Proj("lonlat"), Options());
    c.frw(p1); c.frw(p2);     c.frw(p3); c.frw(p4);
    std::cout << std::fixed 
              << "  " << std::setprecision(14)<< p1.x << " " << std::setprecision(14)<< p1.y 
              << "  " << std::setprecision(14)<< p2.x << " " << std::setprecision(14)<< p2.y 
              << "  " << std::setprecision(14)<< p3.x << " " << std::setprecision(14)<< p3.y 
              << "  " << std::setprecision(14)<< p4.x << " " << std::setprecision(14)<< p4.y ;
//    std::cout << p1.x-p4.x << " "
//              << p2.x-p3.x << " "
//              << p1.y-p2.y << " "
//              << p3.y-p4.y << " ";

  }
}
