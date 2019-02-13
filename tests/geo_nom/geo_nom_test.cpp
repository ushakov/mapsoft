#include "geo/geo_nom.h"
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;
using namespace convs;

int
main(){
  dPoint p1(37.405372, 55.798710);

  assert(pt_to_nom(p1, 50000) == "n37-003-4");
  assert(pt_to_nom(p1, 100000) == "n37-003");
  assert(pt_to_nom(p1, 200000) == "n37-02");
  assert(pt_to_nom(p1, 500000) == "n37-1");
  assert(pt_to_nom(p1, 1000000) == "n37");

  // double maps

  dPoint p2(34.567589, 67.768951);
  assert(pt_to_nom(p2, 50000) == "q36-010-3-4");
  assert(pt_to_nom(p2, 100000) == "q36-009-010");
  assert(pt_to_nom(p2, 200000) == "q36-05-06");
  assert(pt_to_nom(p2, 500000) == "q36-1-2");
  assert(pt_to_nom(p2, 1000000) == "q35-36");

  assert( nom_shift("n37-144-3", iPoint(1,0)) == "n37-144-4");
  assert( nom_shift("n37-144-3", iPoint(2,0)) == "n38-133-3");
  assert( nom_shift("n37-144-3", iPoint(0,-1)) == "m37-012-1");
  assert( nom_shift("n37-144-3", iPoint(2,-1)) == "m38-001-1");

  vector<string> names = range_to_nomlist(dRect(35,59,2,2), 200000);
  for (vector<string>::const_iterator i=names.begin(); i!=names.end(); i++)
    cout << *i << "\t" << nom_to_range(*i) << "\n";
  return 0;
}