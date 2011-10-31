// Привязки специальных карт и снимков

#include "geo_refs.h"
#include "geo_convs.h"
#include "loaders/image_google.h"
#include "loaders/image_ks.h"
#include "2d/line_utils.h"

g_map mk_tmerc_ref(const dLine & points, double u_per_m, bool yswap){
  g_map ref;

  if (points.size()<3){
    std::cerr << "error in mk_tmerc_ref: number of points < 3\n";
    return g_map();
  }
  // Get refs.
  // Reduce border to 5 points, remove last one.
  dLine refs = points;
  refs.push_back(*refs.begin()); // to assure last=first
  refs = generalize(refs, -1, 5);
  refs.resize(4);

  // Create lonlat -> tmerc conversion with wanted lon0,
  // convert refs to our map coordinates.
  Options PrO;
  PrO.put<double>("lon0", convs::lon2lon0(points.center().x));
  convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), PrO,
                   Datum("wgs84"), Proj("tmerc"), PrO);
  dLine refs_c(refs);
  cnv.line_frw_p2p(refs_c);
  refs_c *= u_per_m; // to out units
  refs_c -= refs_c.range().TLC();
  double h = refs_c.range().h;

  // swap y if needed
  if (yswap){
    for (int i=0;i<refs_c.size();i++)
      refs_c[i].y = h - refs_c[i].y;
  }

  // add refpoints to our map
  for (int i=0;i<refs.size();i++){
    ref.push_back(g_refpoint(refs[i], refs_c[i]));
  }

  ref.map_proj=Proj("tmerc");

  // Now we need to convert border to map units.
  // We can convert them by the same way as refs, but
  // this is unnecessary duplicating of non-trivial code.
  // So we constract map2pt conversion from our map.

  // Set map border
  convs::map2pt brd_cnv(ref, Datum("wgs84"), Proj("lonlat"));
  ref.border = brd_cnv.line_bck(points);
  ref.border.push_back(*ref.border.begin()); // to assure last=first
  ref.border = generalize(ref.border, 1, -1); // 1 unit accuracy
  ref.border.resize(ref.border.size()-1);

  return ref;
}

g_map ref_google(int scale){
   g_map ret;
   ret.map_proj = Proj("google");
   ret.comm = "maps.google.com";

   if (scale<google::google_scale_min) scale=google::google_scale_min;
   if (scale>google::google_scale_max) scale=google::google_scale_max;

   double maxlat = 360/M_PI*atan(exp(M_PI)) - 90;
   double width = 1<<(8+scale-1);

   ret.push_back(g_refpoint(-180,maxlat,  0,0));
   ret.push_back(g_refpoint( 180,maxlat,  width,0));
   ret.push_back(g_refpoint( 180,-maxlat, width,width));
   ret.push_back(g_refpoint(-180,-maxlat, 0,width));

   ret.border.push_back(dPoint(0,0));
   ret.border.push_back(dPoint(width,0));
   ret.border.push_back(dPoint(width,width));
   ret.border.push_back(dPoint(0,width));
   return ret;
}

g_map ref_ks_old(int scale){
   g_map ret;
   ret.map_proj = Proj("ks");
   ret.comm = "new.kosmosnimki.ru";

   if (scale<ks::ks_scale_min) scale=ks::ks_scale_min;
   if (scale>ks::ks_scale_max) scale=ks::ks_scale_max;
 
   // Datum тут должен быть wgs, 
   // в Proj(ks) параметры эллипсоида записаны жестко
   convs::pt2pt c1(Datum("wgs84"), Proj("ks"), Options(), Datum("wgs84"), Proj("lonlat"), Options());

   dPoint p(5 * (1<<14) * 256, 5 * (1<<14) * 256);
   c1.frw(p);
   double width = 5*256*(1<<(scale-3));
   double maxlat=p.y;
   double maxlon=p.x;

   ret.push_back(g_refpoint(0,  p.y, 0,0));
   ret.push_back(g_refpoint(p.x,p.y, width,0));
   ret.push_back(g_refpoint(p.x,0,   width,width));
   ret.push_back(g_refpoint(0,  0,   0,width));

   ret.border.push_back(dPoint(0,0));
   ret.border.push_back(dPoint(width,0));
   ret.border.push_back(dPoint(width,width));
   ret.border.push_back(dPoint(0,width));
   return ret;
}

g_map ref_ks(int scale){
   g_map ret;
   ret.map_proj = Proj("ks");
   ret.comm = "new.kosmosnimki.ru";

   if (scale<ks::ks_scale_min) scale=ks::ks_scale_min;
   if (scale>ks::ks_scale_max) scale=ks::ks_scale_max;

   double width = 256*(1<<(scale-1));
   double maxlat = 85.08405905;

   ret.push_back(g_refpoint(0,maxlat,0,0));
   ret.push_back(g_refpoint(180,maxlat,width,0));
   ret.push_back(g_refpoint(180,0,width,width));
   ret.push_back(g_refpoint(0,0,0,width));

   // ручная поправка :(((
   ret += dPoint(6.0/(1<<(14-scale)),0);

   ret.border.push_back(dPoint(0,0));
   ret.border.push_back(dPoint(width,0));
   ret.border.push_back(dPoint(width,width));
   ret.border.push_back(dPoint(0,width));

   return ret;
}
