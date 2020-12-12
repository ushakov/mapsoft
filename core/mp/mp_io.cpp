#include "utils/spirit_utils.h"
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <boost/spirit/include/classic_insert_at_actor.hpp>
#include <boost/spirit/include/classic_clear_actor.hpp>
#include <boost/spirit/include/classic_erase_actor.hpp>

#include <iomanip>
#include <fstream>
#include <cstring>

#include "mp_io.h"
#include "utils/iconv_utils.h"

namespace mp {

using namespace std;
using namespace boost::spirit::classic;

bool read(const char* filename, mp_world & world, const Options & opts){

  mp_world ret;
  mp_object o, o0;
  dPoint pt;

  dLine   line;
  string         comm, key, val;

  rule_t key_ch = anychar_p - eol_p - '=';
  rule_t ch = anychar_p - eol_p;
  rule_t comment = ch_p(';') >> (*ch)[assign_a(comm)] >> eol_p;
  rule_t option  = (*key_ch)[assign_a(key)] >> '=' >> (*ch)[assign_a(val)] >> eol_p;

  rule_t header =
      *(comment[push_back_a(ret.Comment, comm)] | space_p)  >>
      ("[IMG ID]") >> +eol_p >> *(
      ( "ID="              >> !uint_p[assign_a(ret.ID)]            >> +eol_p) |
      ( "Name="            >> (*ch)[assign_a(ret.Name)]            >> +eol_p) |
      ( option[erase_a(ret.Opts, key)][insert_at_a(ret.Opts, key, val)] )
    ) >> "[END-IMG ID]" >> +eol_p;

    rule_t pt_r = ch_p('(') 
		  >> real_p[assign_a(pt.y)] >> ',' 
		  >> real_p[assign_a(pt.x)]
                  >> ch_p(')');

    const string sp="POI", sl="POLYLINE", sa="POLYGON";
    rule_t object =
      eps_p[assign_a(o,o0)] >> *(comment[push_back_a(o.Comment, comm)] | space_p) >>
      ch_p('[') >>
       ((str_p("POI") | "RGN10" | "RGN20")[assign_a(o.Class, sp)] |
        (str_p("POLYLINE") | "RGN40")[assign_a(o.Class, sl)] |
        (str_p("POLYGON")  | "RGN80")[assign_a(o.Class, sa)]
       ) >>
      ch_p(']') >> +eol_p >>

      *(( "Type=0x"   >> hex_p[assign_a(o.Type)]  >> +eol_p) |
        ( "Label="    >> (*ch)[assign_a(o.Label)] >> +eol_p) |
        ( "EndLevel=" >> uint_p[assign_a(o.EL)]   >> +eol_p) |
        ( "Endlevel=" >> uint_p[assign_a(o.EL)]   >> +eol_p) |
        ( "Levels="   >> uint_p[assign_a(o.EL)]   >> +eol_p) |

        ((str_p("Data") | "Origin") >> uint_p[assign_a(o.BL)][clear_a(line)] >> "=" >>
           (+eol_p |
           (pt_r[push_back_a(line, pt)] >>
             *(',' >> pt_r[push_back_a(line, pt)]) >> +eol_p) [push_back_a(o,line)]
           )
        ) |

        ( option[erase_a(ret.Opts, key)][insert_at_a(o.Opts, key, val)] )

        ) >>
      "[END" >> *(ch-ch_p(']')) >> ch_p(']') >> eol_p[push_back_a(ret,o)];

    rule_t main_rule = header >>
        *object >> *space_p >> *(+comment >> *space_p);
    // комментарии после объектов - теряются!

    if (!parse_file("mp::read", filename, main_rule)) return false;

    // converting some fields to UTF8
    string codepage="1251";
    codepage=ret.Opts.get("CodePage", codepage);   // override by file setting
    codepage=opts.get("mp_in_codepage", codepage); // override by user setting
    IConv cnv("CP" + codepage);

    for (mp_world::iterator i = ret.begin(); i != ret.end(); i++){
      i->Label = cnv.to_utf8(i->Label);
      for (Options::iterator o=i->Opts.begin(); o!=i->Opts.end(); o++){
        o->second=cnv.to_utf8(o->second);
      }
    }

    ret.Name = cnv.to_utf8(ret.Name);
    for (Options::iterator o=ret.Opts.begin(); o!=ret.Opts.end(); o++){
      o->second=cnv.to_utf8(o->second);
    }

    for (vector<string>::iterator
                  c = ret.Comment.begin(); c != ret.Comment.end(); c++){
      *c = cnv.to_utf8(*c);
    }

    // removing bad objects
    mp_world::iterator i = ret.begin();
    while (i!= ret.end()){

      mp_object::iterator l = i->begin();
      while (l!= i->end()){

        if (l->size()==0){
          std::cerr << "MP:read warning: deleting empty object segment\n";
          l = i->erase(l);
          continue;
        }
        if ((i->Class!="POI") && (l->size()==1)){
          std::cerr << "MP:read warning: deleting line segment with 1 point\n";
          l = i->erase(l);
          continue;
        }
        if ((i->Class=="POI") && (l->size()>1)){
          std::cerr << "MP:read warning: cropping POI segment with > 1 points\n";
          l->resize(1);
        }
        l++;
      }
      if (i->size()==0){
        std::cerr << "MP:read warning: deleting empty object\n";
        i = ret.erase(i);
      }
      else i++;
    }

    // merging world with ret
    mp::mp_world tmp = world;
    world=ret;
    world.insert(world.begin(), tmp.begin(), tmp.end());

    return true;
}

bool write(std::ostream & out, const mp_world & world, const Options & opts){

  // converting some fields from UTF8 to default codepage
  // setting CodePage to default_codepage;

  string codepage="1251";
  codepage=world.Opts.get("CodePage", codepage);     // override by file setting
  codepage=opts.get("mp_out_codepage", codepage); // override by user setting
  IConv cnv("CP" + codepage);

  for (vector<string>::const_iterator c = world.Comment.begin();
       c!=world.Comment.end(); c++) out << ";" << cnv.from_utf8(*c) << "\n";
//cerr <<"Name" << world.Name << "->" << cnv.from_utf8(world.Name) << "\n";
  out << setprecision(6) << fixed
      << "\r\n[IMG ID]"
      << "\r\nID="              << world.ID
      << "\r\nName="            << cnv.from_utf8(world.Name);

  // we need options to be printed in right order (Levels before Level0...)
  Options lopts=world.Opts; //copy options

  const char* names[] = {"ID","Name","Elevation","Preprocess","CodePage",
    "LblCoding","TreSize","TreMargin","RgnLimit","POIIndex","Levels",
    "Level0","Level1","Level2","Level3","Level4","Level5","Level6",
    "Zoom0","Zoom1","Zoom2","Zoom3","Zoom4","Zoom5","Zoom6"};

  for (size_t i=0; i<sizeof(names)/sizeof(char*); i++){
    if (lopts.count(names[i])>0){
      if (strcmp(names[i],"CodePage")==0)
        out << "\r\nCodePage=" << codepage; // use our new codepage
      else
        out << "\r\n" << names[i] << "=" << cnv.from_utf8(lopts[names[i]]);
      lopts.erase(names[i]);
    }
  }
  // other options
  for (Options::const_iterator o=lopts.begin(); o!=lopts.end(); o++){
    out << "\r\n" << o->first << "=" << cnv.from_utf8(o->second);
  }

  out << "\r\n[END-IMG ID]\r\n";

  for (mp_world::const_iterator i=world.begin(); i!=world.end(); i++){
    for (vector<string>::const_iterator c = i->Comment.begin();
         c!=i->Comment.end(); c++) out << ";" << cnv.from_utf8(*c) << "\n";
    out << "\r\n[" << i->Class << "]"
        << "\r\nType=0x"     << setbase(16) << i->Type << setbase(10);
    if (i->Label != "") out << "\r\nLabel=" << cnv.from_utf8(i->Label);
    if (i->EL != 0)     out << "\r\nEndLevel=" << i->EL;

    for (Options::const_iterator o=i->Opts.begin(); o!=i->Opts.end(); o++){
      out << "\r\n" << o->first << "=" << cnv.from_utf8(o->second);
    }

    for (dMultiLine::const_iterator l=i->begin(); l!=i->end(); l++){
      out << "\r\nData" << i->BL << "="; 
      for (size_t j=0; j<l->size(); j++){
        out << ((j!=0)?",":"") << "(" 
            << (*l)[j].y << "," << (*l)[j].x << ")";
      }
    }

    out << "\r\n[END]\r\n";
  }
  return true;
}

bool write(const string & file, const mp_world & world, const Options & opts){
  ofstream out(file.c_str());
  if (!out) {
    cerr << "Error: can't open mp file \"" << file << "\"\n";
    return false;
  }
  if (!write(out, world, opts)) {
    cerr << "Error: can't write to mp file \"" << file << "\"\n";
    return false;
  }
  return true;
}


} //namespace
