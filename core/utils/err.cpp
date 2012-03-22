#include "err.h"

using namespace std;

MapsoftErr::MapsoftErr(const string & id):i(id){
}

MapsoftErr::MapsoftErr(const MapsoftErr & o):i(o.i){
  s << o.s.str();
}

string
MapsoftErr::str() const {
  return s.str();
}

string
MapsoftErr::id() const {
  return i;
}

