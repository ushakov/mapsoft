#include "../lib2d/point.h"
#include "../lib2d/line.h"
#include "../lib2d/rect.h"
#include "../libgeo/geo_types.h"
#include <boost/lexical_cast.hpp>

template<typename T1, typename T2>
bool cast_test(const T1 & p1, const T2 & p2, bool good){
  T2 p;

  try{ p = boost::lexical_cast<T2>(p1);}
  catch(boost::bad_lexical_cast c){
    if (good){
      std::cerr << "Error: bad cast for \"" << p1 << "\"\n";
      return false;
    }
    else return true;
  }

  if (!good){
    std::cerr << "Error: good cast for \"" << p1 << "\"\n";
    return false;
  }

  if (p!=p2) {
    std::cerr << "Error: \"" << p1 << "\" != \"" << p2 << "\"\n";
    return false;
  }
  return true;
}


int main(){

std::cerr << " * Points\n";

  Point<double> p(1,2);
  cast_test("1,2",   p, true);
  cast_test("1, 2",   p, true);
  cast_test("1 ,2",   p, true);
  cast_test("1 ,2 ",   p, true);
  cast_test(" 1 ,2 ",   p, true);
  cast_test("1,2,",  p, false);
  cast_test("1 ,2,", p, false);
  cast_test("1 2",   p, false);
  cast_test("1 2,3", p, false);

std::cerr << " * Lines\n";

  Line<double> l1,l2;
  l1.push_back(Point<double>(1.1,2.2));
  l2.push_back(Point<double>(1.1,2.2));
  l2.push_back(Point<double>(3.1,4.2));

  cast_test("1.1,2.2",   l1, true);
  cast_test("1.1,2.2,3.1,4.2",  l2, true);
  cast_test("1.1, 2.2, 3.1,4.2", l2, true);
  cast_test(" 1.1 , 2.2 ,3.1 ,4.2 ", l2, true);
  cast_test("1,2,",      l1, false);
  cast_test("1 ,2,",     l1, false);
  cast_test("1 2",       l1, false);
  cast_test("1 2,3",     l1, false);
  cast_test("1.1 1.1,2.2",     l1, false);
  cast_test("1.1,2.2,",     l1, false);

std::cerr << " * Rects\n";

  Rect<double> r1(100,110,    12,23);
  Rect<double> r2(-100, -120, 12,23);
  cast_test("12x23+100+110",   r1, true);
  cast_test("12x23-100-120",   r2, true);
  cast_test("12x23+-100+-120", r2, true);
  cast_test("12x23+-100+-120,", r2, false);

  cast_test(Point<double>(1.1,2.2), l1, true);

std::cerr << " * Datum\n";

  cast_test("wgs84", Datum("wgs84"), true);
  cast_test("wgs84 ", Datum("wgs84"), false);


std::cerr << " * XML format:\n";

  Enum::output_fmt=Enum::xml_fmt;
  std::cerr << "Datum(\"aaa\") -> "     << Datum("aaa")     << "\n";
  std::cerr << "Datum(\"pulkovo\") -> " << Datum("pulkovo") << "\n";
  std::cerr << "Datum(\"wgs84\") -> "   << Datum("wgs84")   << "\n";
  std::cerr << "Datum(\"10\") -> "      << Datum("10")      << "\n";
  std::cerr << "Datum(\"\") -> "        << Datum("")        << "\n";
  std::cerr << "Datum() -> "            << Datum()          << "\n";
  std::cerr << "Datum(\"0x10\") -> "    << Datum("0x10")    << "\n";

  std::cerr << "Proj(\"aaa\") -> "     << Proj("aaa")     << "\n";
  std::cerr << "Proj(\"lonlat\") -> "  << Proj("lonlat") << "\n";
  std::cerr << "Proj(\"tmerc\") -> "   << Proj("tmerc")   << "\n";
  std::cerr << "Proj(\"10\") -> "      << Proj("10")      << "\n";
  std::cerr << "Proj(\"\") -> "        << Proj("")        << "\n";
  std::cerr << "Proj() -> "            << Proj()          << "\n";
  std::cerr << "Proj(\"0x10\") -> "    << Proj("0x10")    << "\n";

std::cerr << " * OE format:\n";

  Enum::output_fmt=Enum::oe_fmt;
  std::cerr << "Datum(\"pulkovo\") -> " << Datum("pulkovo") << "\n";
  std::cerr << "Datum(\"wgs84\") -> "   << Datum("wgs84")   << "\n";
  std::cerr << "Proj(\"lonlat\") -> "  << Proj("lonlat") << "\n";
  std::cerr << "Proj(\"tmerc\") -> "   << Proj("tmerc")   << "\n";

}