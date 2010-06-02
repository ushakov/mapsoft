#ifndef MP_DATA_H
#define MP_DATA_H

#include <list>
#include <vector>
#include <string>
#include <iostream>

#include "lib2d/line.h"
#include "utils/options.h"

namespace mp {

    struct mp_object : dMultiLine{
        std::string          Class;
        int                  Type;
        std::string          Label;
        std::vector<std::string> Comment;
        int                  BL,EL; // begin/end level
        Options              Opts;  // other options

        mp_object(){
          Class="POI"; Type=-1; BL=0; EL=0;
        }

        bool operator== (const mp_object & o) const{
          if (size()!=o.size()) return false;
          dMultiLine::const_iterator i,j;
          for (i = begin(), j=o.begin(); (i!=end()) && (j!=o.end()) ; i++, j++)
            if (*i!=*j) return false;
          return (
            (Class==o.Class) && (Type==o.Type) && (Label==o.Label) &&
            (BL==o.BL) && (EL==o.EL) && (Comment==o.Comment) && (Opts==o.Opts) );
        }
        mp_object & operator= (const dLine & v){
          clear();
          push_back(v);
          return *this;
        }
        // set points from line
//        void set_points(const dLine & v);

    };

    struct mp_world:std::list<mp_object>{
        int         ID;
        std::string Name;
        std::vector<std::string> Comment;
        Options     Opts;  // other options

        mp_world(){
          ID=0;
          Opts["CodePage"]        = "1251";
          Opts["Elevation"]       = "M";
          Opts["Preprocess"]      = "F";
          Opts["LblCoding"]       = "9";
          Opts["TreSize"]         = "511";
          Opts["TreMargin"]       = "0.0";
          Opts["RgnLimit"]        = "127";
          Opts["Transparent"]     = "N";
          Opts["POIIndex"]        = "N";
          Opts["PolygonEvaluate"] = "Y";
          Opts["MG"]              = "Y";
          Opts["Levels"] = "4";
          Opts["Level0"] = "22";
          Opts["Level1"] = "21";
          Opts["Level2"] = "19";
          Opts["Level3"] = "17";
          Opts["Zoom0"]  = "0";
          Opts["Zoom1"]  = "1";
          Opts["Zoom2"]  = "2";
          Opts["Zoom3"]  = "3";
        }
    };

} // namespace
#endif
