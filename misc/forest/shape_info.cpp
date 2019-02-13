#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <list>
#include <stdexcept>
#include <cmath>

#include "srtm/srtm3.h"
#include "2d/line.h"
#include "2d/line_rectcrop.h"
#include "geo/geo_nom.h"

#include "2d/line_utils.h"
#include "2d/line_polycrop.h"
#include "2d/point_int.h"

#include "geo_io/geofig.h"
#include "geo/geo_data.h"
#include "geo/geo_convs.h"

#include "loaders/image_tiff.h"
#include "loaders/image_jpeg.h"
#include "geo_io/io.h"

#include <shapefil.h>

// Print shape file structure

using namespace std;

void usage(){
    cerr << "usage: shape_info <shape_prefix>\n";
    exit(0);
}

int
main(int argc, char** argv){
  if (argc < 2) usage();

  string pref = argv[1];

  // open shape-file
  SHPHandle sh  = SHPOpen( (pref).c_str(), "rb");

  int num, type;
  double mmin[4],mmax[4];
  SHPGetInfo(sh, &num, &type, mmin, mmax);
  cout << "number of entities/structures: " << num << "\n";
  cout << "shapetype: "
     << (type==SHPT_POINT?   "point":"")
     << (type==SHPT_ARC?     "arc":"")
     << (type==SHPT_POLYGON? "polygon":"")
     << (type==SHPT_MULTIPOINT? "multipoint":"")
     << "\n";
  cout
     << "min x: " << mmin[0]
     << "min y: " << mmin[1]
     << "min z: " << mmin[2]
     << "min m: " << mmin[3]
     << "\n";
  cout
     << "max x: " << mmax[0]
     << "max y: " << mmax[1]
     << "max z: " << mmax[2]
     << "max m: " << mmax[3]
     << "\n";

  DBFHandle dbf = DBFOpen( (pref+".dbf").c_str(), "rb");
  cout << "DBF info:";
  cout << "  DBFGetFieldCount: " << DBFGetFieldCount(dbf) << "\n";
  cout << "  DBFGetRecordCount: " << DBFGetRecordCount(dbf) << "\n";

  cout << "  Fields:";
  cout << "    # n name width decimals\n";
  vector<string> fields(DBFGetFieldCount(dbf));
  for (int i=0; i<DBFGetFieldCount(dbf); i++){
    char fn[12];
    int w,d;
    DBFFieldType t = DBFGetFieldInfo(dbf, i, fn, &w, &d);
    fields[i] = fn;
    cout << "    " << i << " " << fn << " " << w << " " << d << "\n";
  }

  for (int i=0; i<num; i++){
    SHPObject *o = SHPReadObject(sh, i);
    cout << "Object: " << i << "\n";
    cout << "  parts: " << o->nParts << "\n";
/*
    dMultiLine ml1;
    for (int p = 0; p<o->nParts; p++){
      dLine l;
      int j1 = o->panPartStart[p];
      int j2 = (p==o->nParts-1 ? o->nVertices : o->panPartStart[p+1]);
      for (int j=j1; j<j2; j++) l.push_back(dPoint(o->padfX[j],o->padfY[j]));
      dMultiLine ml2 = rect_crop_ml(range,l,true);
      ml1.insert(ml1.end(), ml2.begin(), ml2.end());
    }
    join_polygons1(ml1);
*/
    for (int j=0; j<fields.size(); j++){
      char fn[12];
      int w,d;
      string attr = DBFReadStringAttribute(dbf, i, j);
      cout << "  Field " << j << ": " << fields[j] << " = " <<  attr << "\n";
    }
    SHPDestroyObject(o);
  }
  DBFClose(dbf);
  SHPClose(sh);
  return 0;
}
