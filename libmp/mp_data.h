#ifndef MP_DATA_H
#define MP_DATA_H

#include <list>
#include <vector>
#include <string>
#include <iostream>

#include "../lib2d/line.h"
#include "../utils/options.h"

namespace mp {

    struct mp_object : Line<double>{
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
          for (int i = 0; i<size(); i++) if ((*this)[i]!=o[i]) return false;
          return (
            (Class==o.Class) && (Type==o.Type) && (Label==o.Label) &&
            (BL==o.BL) && (EL==o.EL) && (Comment==o.Comment) && (Opts==o.Opts) );
        }
        mp_object & operator= (const Line<double> v){
          clear();
          for (Line<double>::const_iterator i=v.begin(); i!=v.end(); i++)
            push_back(*i);
          return *this;
        }
        // set points from line
        void set_points(const Line<double> & v);
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
          Opts["Transparent"]     = "Y";
          Opts["POIIndex"]        = "N";
          Opts["PolygonEvaluate"] = "Y";
          Opts["MG"]              = "Y";
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
