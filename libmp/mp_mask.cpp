#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

#include "mp_mask.h"

namespace mp {

using namespace std;
using namespace boost::spirit;

// ��������� mp-������ �� ������ obj, ��������� ��� ��-��������� �� �����
mp_object make_object(const mp_object & obj, const std::string & mask){
  mp_object o=obj; // �����
  if (!parse(mask.c_str(),
    *blank_p >> ((str_p("POI") | "POLYLINE" | "POLYGON")[assign_a(o.Class)] | "*") >>
    +blank_p >> (("0x" >> hex_p[assign_a(o.Type)]) | "*") >>
    +blank_p >> (uint_p[assign_a(o.BL)] | "*") >>
    +blank_p >> (uint_p[assign_a(o.EL)] | "*") >> *blank_p ).full)
      cerr << "Can't parse mp mask: " << mask << "\n";
  return o;
}
// ��������� mp-������ �� ������ ������� �� ���������
mp_object make_object(const std::string & mask){
  return make_object(mp_object(), mask);
}
// ���������, ������������� �� ������ �����
bool test_object(const mp_object & o, const std::string & mask){
  return make_object(o, mask)==o;
}

}  //namespace
