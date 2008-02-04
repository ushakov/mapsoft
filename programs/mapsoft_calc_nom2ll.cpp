#include <iomanip>
#include "../geo_io/io.h"
#include "../geo_io/geo_convs.h"

int main(int argc, char **argv){
  if (argc<=1) std::cout << "lat: 0 0 lon: 0 0";
  else { 
    Rect<double> brd = filters::nom_range(argv[1]);
    Point<double> p1 = brd.TLC(), p2 = brd.BRC();
    convs::pt2pt c(Datum("pulkovo"), Proj("lonlat"), Options(), 
                  Datum("wgs84"), Proj("lonlat"), Options());
    c.frw(p1); c.frw(p2);
    std::cout << std::fixed << std::setw(8)
              << " lat: " << p1.y << " " << p2.y 
              << " lon: " << p1.x << " " << p2.x; 
  }
}
