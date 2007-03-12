#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "../utils/point.h"

namespace mp {


    struct mp_object{
        std::string          Class; 
        int                  Type;
        std::string          Label;
	std::vector<int>     Levels;
	int                  EndLevel;
	std::vector<double>  X,Y;
	mp_object(){ 
	  Class="POI"; Type=0; EndLevel=0; 
        }
    };

    struct mp_world:std::vector<mp_object>{
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
        }
    };

    mp_world read(const char* filename);
    bool write(std::ostream & out, const mp_world & world);
}
