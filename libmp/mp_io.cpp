#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>

#include <iomanip>

#include "mp_io.h"
#include "../utils/spirit_utils.h"
#include "../utils/iconv_utils.h"

namespace mp {

using namespace std;
using namespace boost::spirit;

// Название charset'a получается добавлением "CP"
string default_codepage = "1251";

bool read(const char* filename, mp_world & world){

  mp_world ret;
  mp_object o, o0;
  int l=0;
  Point<double> pt;

  rule_t ch = anychar_p - eol_p;
  rule_t comment = (ch_p(';') >> (*ch)[push_back_a(o.Comment)] >> eol_p) | space_p;

  rule_t header = 
    *((ch_p(';') >> (*ch)[push_back_a(ret.Comment)] >> eol_p) | space_p) >>
      ("[IMG ID]") >> eol_p >> *(
      ( "ID="         >> !uint_p[assign_a(ret.ID)]         >> eol_p) |
      ( "Name="       >> (*ch)[assign_a(ret.Name)]        >> eol_p) |
      ( "Elevation="  >> (*ch)[assign_a(ret.Elevation)]   >> eol_p) |
      ( "Preprocess=" >> (*ch)[assign_a(ret.Preprocess)]  >> eol_p) |
      ( "CodePage="   >> (*ch)[assign_a(ret.CodePage)]    >> eol_p) |
      ( "LblCoding="  >> !uint_p[assign_a(ret.LblCoding)]  >> eol_p) |
      ( "TreSize="    >> !uint_p[assign_a(ret.TreSize)]    >> eol_p) |
      ( "TreMargin="  >> !ureal_p[assign_a(ret.TreMargin)] >> eol_p) |
      ( "RgnLimit="   >> !uint_p[assign_a(ret.RgnLimit)]   >> eol_p) |
      ( "Transparent=">> (*ch)[assign_a(ret.Transparent)] >> eol_p) |
      ( "POIIndex="   >> (*ch)[assign_a(ret.POIIndex)]    >> eol_p) |
      ( "Copyright="  >> (*ch)[assign_a(ret.Copyright)]    >> eol_p) |
      ( "Levels="     >> !uint_p >> eol_p) |
      ( "Level" >> uint_p[assign_a(l)] >> "=" 
                >> !uint_p[insert_at_a(ret.Levels,l)] >> eol_p) |
      ( "Zoom"  >> uint_p[assign_a(l)] >> "=" 
                >> !uint_p[insert_at_a(ret.Zooms,l)] >> eol_p)
    ) >> "[END-IMG ID]" >> eol_p;

    rule_t pt_r = ch_p('(') 
		  >> real_p[assign_a(pt.y)] >> ',' 
		  >> real_p[assign_a(pt.x)] 
                  >> ch_p(')')[push_back_a(o, pt)];

    rule_t object = *comment >> ch_p('[') >> (
      (str_p("POI")      | "RGN10" | "RGN20")[assign_a(o.Class, "POI")] |
      (str_p("POLYLINE") | "RGN40")[assign_a(o.Class, "POLYLINE")] |
      (str_p("POLYGON")  | "RGN80")[assign_a(o.Class, "POLYGON")] ) 
      >> ch_p(']') >> eol_p 
      >> *(
        ( "Type=0x"   >> hex_p[assign_a(o.Type)]      >> eol_p) |
        ( "Label="    >> (*ch)[assign_a(o.Label)]     >> eol_p) |
        ( "EndLevel=" >> uint_p[assign_a(o.EL)] >> eol_p) |
        ( "Endlevel=" >> uint_p[assign_a(o.EL)] >> eol_p) |
        ( "Levels="   >> uint_p[assign_a(o.EL)] >> eol_p) |
        ( "DirIndicator="   >> uint_p[assign_a(o.DirIndicator)] >> eol_p) |
        ( (str_p("Data") | str_p("Origin")) >> uint_p[assign_a(o.BL)] >> "="
           >> pt_r >> *(',' >> pt_r) >> eol_p)[push_back_a(ret,o)][clear_a(o)] 
      ) >> "[END" >> *(ch-ch_p(']')) >> ch_p(']') >> eol_p;


    rule_t main_rule = header >>
        *( eps_p[assign_a(o,o0)] >> object )
        >> *space_p >> *(+comment >> *space_p);
    // комментарии после объектов - теряются!

    if (!parse_file("mp::read", filename, main_rule)) return false;

    // converting some fields to UTF8
    IConv cnv(("CP" + ret.CodePage).c_str(), ("CP" + default_codepage).c_str());
    ret.Name = cnv.to_utf(ret.Name);
    for (mp_world::iterator i = ret.begin(); i != ret.end(); i++){
      i->Label = cnv.to_utf(i->Label);
      for (vector<string>::iterator
                  c = i->Comment.begin(); c != i->Comment.end(); c++){
        *c = cnv.to_utf(*c);
      }
    }
    for (vector<string>::iterator
                  c = world.Comment.begin(); c != world.Comment.end(); c++){
      *c = cnv.to_utf(*c);
    }

    // merging world with ret
    world.swap(ret);
    world.insert(world.begin(), ret.begin(), ret.end());

    return true;
}



bool write(std::ostream & out, const mp_world & world){

  // converting some fields from UTF8 to default codepage
  // setting CodePage to default_charset;

  IConv cnv(("CP"+default_codepage).c_str(), ("CP"+default_codepage).c_str());

  for (vector<string>::const_iterator c = world.Comment.begin();
       c!=world.Comment.end(); c++) out << ";" << cnv.from_utf(*c) << "\n";
  out << setprecision(6) << fixed
      << "\r\n[IMG ID]" 
      << "\r\nID="         << world.ID 
      << "\r\nName="       << cnv.from_utf(world.Name)
      << "\r\nElevation="  << world.Elevation
      << "\r\nPreprocess=" << world.Preprocess
      << "\r\nCodePage="   << default_codepage
      << "\r\nLblCoding="  << world.LblCoding
      << "\r\nTreSize="    << world.TreSize
      << "\r\nTreMargin="  << world.TreMargin
      << "\r\nRgnLimit="   << world.RgnLimit
      << "\r\nTransparent="<< world.Transparent
      << "\r\nPOIIndex="   << world.POIIndex
      << "\r\nCopyright="  << world.Copyright
      << "\r\nLevels="     << world.Levels.size();
  map<int,int>::const_iterator l;
  for (l=world.Levels.begin(); l!=world.Levels.end(); l++)
     out << "\r\nLevel" << l->first << "=" << l->second;
  for (l=world.Zooms.begin(); l!=world.Zooms.end(); l++)
     out << "\r\nZoom" << l->first << "=" << l->second;
  out << "\r\n[END-IMG ID]\r\n";

  for (mp_world::const_iterator i=world.begin(); i!=world.end(); i++){
    for (vector<string>::const_iterator c = i->Comment.begin();
         c!=i->Comment.end(); c++) out << ";" << cnv.from_utf(*c) << "\n";
    out << "\r\n[" << i->Class << "]"
        << "\r\nType=0x"     << setbase(16) << i->Type << setbase(10);
    if (i->Label != "") out << "\r\nLabel=" << cnv.from_utf(i->Label);
    if (i->EL != 0)     out << "\r\nLevels=" << i->EL;
    if (i->DirIndicator != 0) out << "\r\nDirIndicator=" << i->DirIndicator;

    out << "\r\nData" << i->BL << "="; 
    for (int j=0; j<i->size(); j++){
      out << ((j!=0)?",":"") << "(" 
          << (*i)[j].y << "," << (*i)[j].x << ")";
    }
    out << "\r\n[END]\r\n";
  }
}

} //namespace
