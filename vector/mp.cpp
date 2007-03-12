#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>

#include <vector>
#include <string>
#include <map>
#include <iostream>
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

    rule_t object = ch_p('[') 
      >> (*(ch - ch_p(']')))[assign_a(o.Class)] 
      >> ch_p(']') >> eol_p >> *(
        ( "Type=0x"   >> hex_p[assign_a(o.Type)]      >> eol_p) |
        ( "Label="    >> (*ch)[assign_a(o.Label)]     >> eol_p) |
        ( "Levels="   >> (uint_p)[push_back_a(o.Levels)] 
          >> *(ch_p(',') >> uint_p[push_back_a(o.Levels)]) >> eol_p) |
        ( "EndLevel=" >> uint_p[assign_a(o.EndLevel)] >> eol_p) |
        ( str_p("Data")[push_back_a(o.X,bv)][push_back_a(o.Y,bv)] >> uint_p >> "="
           >> pt_r >> *(',' >> pt_r) >> eol_p) |
        ( str_p("Origin")[push_back_a(o.X,bv)][push_back_a(o.Y,bv)] >> uint_p >> "="
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

  for (int i=0; i<world.size(); i++){
    out << "\r\n[" << world[i].Class << "]"
        << "\r\nType=0x"     << setbase(16) << world[i].Type << setbase(10);
    if (world[i].Label != "")   out << "\r\nLabel=" << world[i].Label;
    if (world[i].EndLevel != 0) out << "\r\nEndLevel=" << world[i].EndLevel;

    if (world[i].Levels.size()!=0) out << "\r\nLevels=";
    for (int j=0; j<world[i].Levels.size(); j++) out << (j?",":"") << world[i].Levels[j];

    if (world[i].X.size()!=world[i].Y.size()){
        cerr << "mp::write: different amount of x and y values\n";
        return false;
    }

    for (int j=0; j<world[i].X.size(); j++){
      int u;
      if ((world[i].X[j]>1e90)||(world[i].Y[j]>1e90)){
        out << "\r\nData0="; u=0;
      } else {
        out << ((u!=0)?",":"") << "(" 
            << world[i].X[j] << "," << world[i].Y[j] << ")";
        u++;
      }
    }
    out << "\r\n[END]\r\n";
  }
}
}
