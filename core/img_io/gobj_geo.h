#ifndef GOBJ_GEO_H
#define GOBJ_GEO_H

#include "gred/gobj.h"
#include "geo/geo_convs.h"

/// Abstract class for objects with geo-reference and options
class GObjGeo : public GObj {
public:
  Options opt;
  convs::map2wgs cnv;
  g_map ref;

  GObjGeo(): ref(get_myref()), cnv(convs::map2wgs(get_myref())) {}

  // get/set options
  void    set_opt(const Options & o){ opt = o; }
  Options get_opt(void) const{ return opt; }

  /// Get some reasonable reference.
  virtual g_map get_myref() const{
    g_map ret;
    ret.map_proj = Proj("lonlat");
    ret.push_back(g_refpoint(0,  90, 0, 0));
    ret.push_back(g_refpoint(180, 0, 180*3600,90*3600));
    ret.push_back(g_refpoint(0,   0, 0, 90*3600));
    return ret;
  }

  virtual void  set_ref(const g_map & ref_){
    ref=ref_; cnv=convs::map2wgs(ref); refresh(); }
  virtual const g_map* get_ref() const {return &ref;}
  virtual convs::map2wgs * get_cnv() {return &cnv;}
  virtual void  rescale(double k){set_ref(ref*k);}

  virtual void refresh() {} /// Refresh layer.
};

#endif
