#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>

#include <iomanip>

#include "mp.h"

namespace mp {

using namespace std;
using namespace boost::spirit;

typedef char                    char_t;
typedef file_iterator <char_t>  iterator_t;
typedef scanner<iterator_t>     scanner_t;
typedef rule <scanner_t>        rule_t;

mp_world read(const char* filename){

  mp_object o, o0;
  mp_world world;
  double x,y;
  const double bv=1e99;
  int l=0;

  // iterators for parsing
  iterator_t first(filename);
  if (!first) { cerr << "can't find file " << filename << '\n'; return world;}
  iterator_t last = first.make_end();

  rule_t ch = anychar_p - eol_p;
  rule_t comment = *((ch_p(';') >> *ch >> eol_p) | eol_p);

  rule_t header = 
    str_p("[IMG ID]") >> eol_p >> *(
      ( "ID="         >> uint_p[assign_a(world.ID)]         >> eol_p) |
      ( "Name="       >> (*ch)[assign_a(world.Name)]        >> eol_p) |
      ( "Elevation="  >> (*ch)[assign_a(world.Elevation)]   >> eol_p) |
      ( "Preprocess=" >> (*ch)[assign_a(world.Preprocess)]  >> eol_p) |
      ( "CodePage="   >> (*ch)[assign_a(world.CodePage)]    >> eol_p) |
      ( "LblCoding="  >> uint_p[assign_a(world.LblCoding)]  >> eol_p) |
      ( "TreSize="    >> uint_p[assign_a(world.TreSize)]    >> eol_p) |
      ( "TreMargin="  >> ureal_p[assign_a(world.TreMargin)] >> eol_p) |
      ( "RgnLimit="   >> uint_p[assign_a(world.RgnLimit)]   >> eol_p) |
      ( "Transparent=">> (*ch)[assign_a(world.Transparent)] >> eol_p) |
      ( "POIIndex="   >> (*ch)[assign_a(world.POIIndex)]    >> eol_p) |
      ( "Levels="     >> uint_p >> eol_p) |
      ( "Level" >> uint_p[assign_a(l)] >> "=" 
                >> uint_p[insert_at_a(world.Levels,l)] >> eol_p) |
      ( "Zoom"  >> uint_p[assign_a(l)] >> "=" 
                >> uint_p[insert_at_a(world.Zooms,l)] >> eol_p)
    ) >> "[END-IMG ID]" >> eol_p;

    rule_t pt_r = ch_p('(') 
		  >> real_p[push_back_a(o.X)] >> ',' 
		  >> real_p[push_back_a(o.Y)] 
                  >> ch_p(')');

    rule_t object = ch_p('[') >> (
      (str_p("POI")      | "RGN10")[assign_a(o.Class, "POI")] |
      (str_p("POLYLINE") | "RGN40")[assign_a(o.Class, "POLYLINE")] |
      (str_p("POLYGON")  | "RGN80")[assign_a(o.Class, "POLYGON")] ) 
      >> ch_p(']') >> eol_p 
      >> *(
        ( "Type=0x"   >> hex_p[assign_a(o.Type)]      >> eol_p) |
        ( "Label="    >> (*ch)[assign_a(o.Label)]     >> eol_p) |
        ( "EndLevel=" >> uint_p[assign_a(o.EL)] >> eol_p) |
        ( "Levels="   >> uint_p[assign_a(o.EL)] >> eol_p) |
        ( str_p("Data")[push_back_a(o.X,bv)][push_back_a(o.Y,bv)] 
           >> uint_p[assign_a(o.BL)] >> "="
           >> pt_r >> *(',' >> pt_r) >> eol_p) |
        ( str_p("Origin")[push_back_a(o.X,bv)][push_back_a(o.Y,bv)] 
           >> uint_p[assign_a(o.BL)] >> "="
           >> pt_r >> *(',' >> pt_r) >> eol_p)
      ) >> "[END" >> (*(ch-ch_p(']'))) 
      >> ch_p(']') >> eol_p[push_back_a(world,o)];
      
    if (!parse(first, last, comment >> header >> 
      *( eps_p[assign_a(o,o0)] >> comment >> object) >> comment).full)
    cerr << "Can't parse file!\n";

    return world;
}

bool write(std::ostream & out, const mp_world & world){
  out << setprecision(6) << fixed
      << "\r\n[IMG ID]" 
      << "\r\nID="         << world.ID 
      << "\r\nName="       << world.Name
      << "\r\nElevation="  << world.Elevation
      << "\r\nPreprocess=" << world.Preprocess
      << "\r\nCodePage="   << world.CodePage
      << "\r\nLblCoding="  << world.LblCoding
      << "\r\nTreSize="    << world.TreSize
      << "\r\nTreMargin="  << world.TreMargin
      << "\r\nRgnLimit="   << world.RgnLimit
      << "\r\nTransparent="<< world.Transparent
      << "\r\nPOIIndex="   << world.POIIndex
      << "\r\nLevels="     << world.Levels.size();
  map<int,int>::const_iterator l;
  for (l=world.Levels.begin(); l!=world.Levels.end(); l++)
     out << "\r\nLevel" << l->first << "=" << l->second;
  for (l=world.Zooms.begin(); l!=world.Zooms.end(); l++)
     out << "\r\nZoom" << l->first << "=" << l->second;
  out << "\r\n[END-IMG ID]\r\n";

  for (mp_world::const_iterator i=world.begin(); i!=world.end(); i++){
    out << "\r\n[" << i->Class << "]"
        << "\r\nType=0x"     << setbase(16) << i->Type << setbase(10);
    if (i->Label != "") out << "\r\nLabel=" << i->Label;
    if (i->EL != 0)     out << "\r\nLevels=" << i->EL;

    if (i->X.size()!=i->Y.size()){
        cerr << "mp::write: different amount of x and y values\n";
        return false;
    }

    for (int j=0; j<i->X.size(); j++){
      int u;
      if ((i->X[j]>1e90)||(i->Y[j]>1e90)){
        out << "\r\nData" << i->BL << "="; u=0;
      } else {
        out << ((u!=0)?",":"") << "(" 
            << i->X[j] << "," << i->Y[j] << ")";
        u++;
      }
    }
    out << "\r\n[END]\r\n";
  }
}

// Построить mp-объект на основе obj, подставив все не-звездочки из маски
mp_object make_object(const mp_object & obj, const std::string & mask){
  mp_object o=obj; // копия
  if (!parse(mask.c_str(),
    *blank_p >> ((str_p("POI") | "POLYLINE" | "POLYGON")[assign_a(o.Class)] | "*") >>
    +blank_p >> (("0x" >> hex_p[assign_a(o.Type)]) | "*") >>
    +blank_p >> (uint_p[assign_a(o.BL)] | "*") >>
    +blank_p >> (uint_p[assign_a(o.EL)] | "*") >> *blank_p ).full)
      cerr << "Can't parse mask!\n";
  return o;
}
// Построить mp-объект на основе объекта по умолчанию
mp_object make_object(const std::string & mask){
  return make_object(mp_object(), mask);
}
// Проверить, соответствует ли объект маске
bool test_object(const mp_object & o, const std::string & mask){
  return make_object(o, mask)==o;
}


}
