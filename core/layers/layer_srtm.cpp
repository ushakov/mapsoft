#include "layer_srtm.h"
#include <sstream>
#include <fstream>

#include "geo/geo_refs.h"

using namespace std;

LayerSRTM::LayerSRTM(){
  mymap=get_myref();
}

g_map
LayerSRTM::get_ref() const {
  return mymap;
}

g_map
LayerSRTM::get_myref() const {
//  return ref_ll(180*1200); // 1200pt/degree
  g_map ret;
  ret.map_proj = Proj("lonlat");
  ret.push_back(g_refpoint(0,  45, 0, 45*1200));
  ret.push_back(g_refpoint(180, 0, 180*1200,0));
  ret.push_back(g_refpoint(0,   0, 0, 90*1200));
  return ret;
}

void
LayerSRTM::set_ref(const g_map & map){
  mymap=map;
}

//iImage
//LayerSRTM::get_image (iRect src){
//  if (rect_intersect(myrange, src).empty()) return iImage(0,0);
//  iImage ret(src.w, src.h, 0);
//  draw(src.TLC(), ret);
//  return ret;
//}

void
LayerSRTM::draw(const iPoint origin, iImage & image){
  iRect src_rect = image.range() + origin;

  convs::map2pt cnv(mymap, Datum("wgs84"),Proj("lonlat"),Options());

  const struct rainbow_data RD[]={
    {30.0, 0xFFFFFF},
    {35.0, 0x00FFFF},
    {40.0, 0x0000FF},
    {45.0, 0xFF00FF},
    {50.0, 0xFF0000},
    {55.0, 0x404040}
  };
  const int RDS=sizeof(RD)/sizeof(rainbow_data);
  const int step=50;

  for (int j=0; j<image.h; j++){
    for (int i=0; i<image.w; i++){
      int c=0xffffff;
      dPoint p0 = origin + iPoint(i,j);
      dPoint px=p0 + dPoint(1,0);
      dPoint py=p0 + dPoint(0,1);

      cnv.frw(p0);
      cnv.frw(px);
      cnv.frw(py);
      int h0 = S.geth4(p0);
      int hx = S.geth4(px);
      int hy = S.geth4(py);

      // holes
      if ((h0 < srtm_min) || (hx < srtm_min) || (hy < srtm_min)){
        c=0xC8C8C8; goto print_colors;
      }

      // contours
      if ((hx/step - h0/step)||(hy/step - h0/step)){
        c=0; goto print_colors;
      }

      { // slopes
        c=get_rainbow(S.slope4(p0), RD, RDS);
        goto print_colors;
      }

      print_colors:
      image.set_na(i,j, c);
    }
  }
}

iRect
LayerSRTM::range() const{
std::cerr << ">>>> " << get_myref().range() << "\n";
  return iRect(
    iPoint(INT_MIN, INT_MIN),
    iPoint(INT_MAX, INT_MAX));
}

