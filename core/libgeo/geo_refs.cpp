// Привязки специальных карт и снимков

#include "geo_refs.h"
#include "geo_convs.h"
#include "loaders/image_google.h"
#include "loaders/image_ks.h"

g_map ref_google(int scale){
   g_map ret;
   ret.map_proj = Proj("google");
   ret.comm = "maps.google.com";

   if (scale<google::google_scale_min) scale=google::google_scale_min;
   if (scale>google::google_scale_max) scale=google::google_scale_max;

   double maxlat = 360/M_PI*atan(exp(M_PI)) - 90;
   double width = 256*(1<<(scale-1));

   ret.push_back(g_refpoint(-180,maxlat,  0,0));
   ret.push_back(g_refpoint( 180,maxlat,  width,0));
   ret.push_back(g_refpoint( 180,-maxlat, width,width));
   ret.push_back(g_refpoint(-180,-maxlat, 0,width));

   convs::pt2pt c(Datum("wgs84"), Proj("lonlat"), Options(),
                  Datum("sphere"), Proj("lonlat"), Options());

   for (g_map::iterator i = ret.begin(); i!=ret.end(); i++) c.bck(*i);

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
