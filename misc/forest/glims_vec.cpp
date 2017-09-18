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


using namespace std;

void usage(){
    cerr << "usage: \n"
     " glims_nom <fig_name> <shape_prefix>\n";
    exit(0);
}

main(int argc, char** argv){
  if (argc < 3) usage();

  std::string fig_name = argv[1];
  std::string pref = argv[2];

  // open fig and find range and fig2wgs conv
  fig::fig_world F;
  if (!fig::read(fig_name.c_str(), F)) {
    std::cerr << "Can't open file, exiting.\n";
    exit(1);
  }
  g_map fig_ref = fig::get_ref(F);
  dRect range = fig_ref.range(); // fig range (lonlat)
  convs::map2wgs cnv(fig_ref);

  // open shape-file
  SHPHandle sh  = SHPOpen( pref.c_str(), "rb");
  DBFHandle dbf = DBFOpen( (pref+".dbf").c_str(), "rb");

  std::cerr << "DBFGetFieldCount: " << DBFGetFieldCount(dbf) << "\n";
  std::cerr << "DBFGetRecordCount: " << DBFGetRecordCount(dbf) << "\n";

  for (int i=0; i<DBFGetFieldCount(dbf); i++){
    char fn[12];
    int w,d;
    DBFFieldType t = DBFGetFieldInfo(dbf, i, fn, &w, &d);
    std::cerr << "  " << i << " " << fn << " " << w << " " << d << "\n";
  }

  int num, type;
  double mmin[4],mmax[4];
  SHPGetInfo(sh, &num, &type, mmin, mmax);
  if (type!=SHPT_POLYGON){ cerr << "type!=SHPT_POLYGON\n"; exit(0); }

  string fig_mask="2 3 0 0 11 11 96 -1 35 0.000 0 0 7 0 0 1";

  fig::fig_object o = fig::make_object("6 0 0 0 0");
  F.push_back(o);

  for (int i=0; i<num; i++){
    SHPObject *o = SHPReadObject(sh, i);
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

    string name = DBFReadStringAttribute(dbf, i, 17);
    string id1 = DBFReadStringAttribute(dbf, i, 0);
    string id2 = DBFReadStringAttribute(dbf, i, 1);

    for(dMultiLine::iterator iv = ml1.begin(); iv!=ml1.end(); iv++){
      if (iv->size()<3) continue;
      dLine l = cnv.line_bck(*iv);
      fig::fig_object o = fig::make_object(fig_mask);
      o.insert(o.end(), l.begin(), l.end());
      o.comment.push_back("");
      o.comment.push_back(name);
      o.comment.push_back(id1);
      o.comment.push_back(id2);
      F.push_back(o);
    }
    SHPDestroyObject(o);
  }



  o = fig::make_object("-6");
  F.push_back(o);

  fig::write(fig_name.c_str(), F);
  exit(0);
}
