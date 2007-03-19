#ifndef MP_H
#define MP_H

#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "../utils/point.h"

namespace mp {

    struct mp_object : std::vector<Point<double> >{
        std::string          Class; 
        int                  Type;
        std::string          Label;
        std::vector<std::string> Comment;
	int                  BL,EL; // begin/end level
	mp_object(){ 
	  Class="POI"; Type=-1; BL=0; EL=0;
        }
        bool operator== (const mp_object & o) const{
          // полное совпадение объектов
          if (size()!=o.size()) return false;
          for (int i = 0; i<size(); i++) if ((*this)[i]!=o[i]) return false;
          return ( 
            (Class==o.Class) && (Type==o.Type) && (Label==o.Label) &&
            (BL==o.BL) && (EL==o.EL) && (Comment==o.Comment));
        }
        mp_object & operator= (const std::vector<Point<double> > v){
          clear();
          for (std::vector<Point<double> >::const_iterator i=v.begin(); i!=v.end(); i++)
            push_back(*i);
          return *this;
        }

    };

    struct mp_world:std::list<mp_object>{
        int         ID;
        std::string Name;
	std::string Elevation;
	std::string Preprocess;
        std::string CodePage;
        int         LblCoding;
        int         TreSize;
        double      TreMargin;
        int         RgnLimit;
        std::string Transparent;
        std::string POIIndex;
        std::vector<std::string> Comment;

        std::map<int,int> Levels;
        std::map<int,int> Zooms;
        mp_world(){
          ID=0;
	  Elevation="M";
	  Preprocess="F";
          CodePage="1251";
          LblCoding=9;
          TreSize=511;
          TreMargin=0.0;
          RgnLimit=127;
          Transparent="Y";
          POIIndex="N";
          Levels[0]=22;
          Levels[1]=21;
          Levels[2]=19;
          Levels[3]=17;
          Zooms[0]=0;
          Zooms[1]=1;
          Zooms[2]=2;
          Zooms[3]=3;
        }
    };

    mp_world read(const char* filename);
    bool write(std::ostream & out, const mp_world & world);

    // Построить mp-объект на основе obj, подставив все не-звездочки из маски
    mp_object make_object(const mp_object & obj, const std::string & mask);
    // Построить mp-объект на основе объекта по умолчанию
    mp_object make_object(const std::string & mask);
    // Проверить, соответствует ли объект маске
    bool test_object(const mp_object & obj, const std::string & mask);

    // Маска:  "POI 0x223 0 1"

}
#endif
