#ifndef GEO_REFS_H
#define GEO_REFS_H

// Привязки специальных карт и снимков

#include "geo_data.h"
#include "../loaders/image_google.h"
#include "../loaders/image_ks.h"

g_map ref_google(int scale){
   g_map ret;
   ret.map_proj = Proj("google");
   ret.comm = "maps.google.com";

   if (scale<google::google_scale_min) scale=google::google_scale_min;
   if (scale>google::google_scale_max) scale=google::google_scale_max;

   double maxlat = 360/M_PI*atan(exp(M_PI/2)) - 90;
   double width = 256*(1<<(scale-1));
   ret.push_back(g_refpoint(0  ,maxlat,  0,0));
   ret.push_back(g_refpoint(360,maxlat,  width,0));
   ret.push_back(g_refpoint(360,-maxlat, width,width));
   ret.push_back(g_refpoint(0  ,-maxlat, 0,width));

   ret.border.push_back(g_point(0,0));
   ret.border.push_back(g_point(width,0));
   ret.border.push_back(g_point(width,width));
   ret.border.push_back(g_point(0,width));
   return ret;
}

g_map ref_ks(int scale){
   g_map ret;
   ret.map_proj = Proj("google");
   ret.comm = "new.kosmosnimki.ru";

   if (scale<ks::ks_scale_min) scale=ks::ks_scale_min;
   if (scale>ks::ks_scale_max) scale=ks::ks_scale_max;

   double maxlat = 360/M_PI*atan(exp(M_PI/2)) - 90;
   double width = 5*256*(1<<(scale-3));
   ret.push_back(g_refpoint(0  ,maxlat, 0,0));
   ret.push_back(g_refpoint(180,maxlat, width,0));
   ret.push_back(g_refpoint(180,0,      width,width));
   ret.push_back(g_refpoint(0  ,0,      0,width));

   ret.border.push_back(g_point(0,0));
   ret.border.push_back(g_point(width,0));
   ret.border.push_back(g_point(width,width));
   ret.border.push_back(g_point(0,width));
   return ret;
}
#endif
