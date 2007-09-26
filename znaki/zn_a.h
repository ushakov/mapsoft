#ifndef ZN_A_H
#define ZN_A_H

// площадной условный знак на карте
// все прочие линейные знаки - его потомки

#include "zn.h"

class zn_a : public zn{
  public:
  zn_a(const std::string & _style = ""){
    descr = "неизвестный науке площадной знак!";
    base_fig.type       = 2;
    base_fig.sub_type   = 3;
    base_fig.area_fill  = 20;
    base_fig.depth      = 4;
    base_mp.Class = "POLYGON";
    style = _style;
  }
};

class zn_a_der : public zn_a{
  public:
  zn_a_der(const std::string & _style = ""){
    descr = "деревня";
    base_fig.pen_color   = 0;
    base_fig.fill_color  = 27;
    base_fig.depth       = 94;
    base_mp.Type         = 0xE;
  }

  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
     "[_polygon]\nType=0xE\nstring1=0x4,деревня\nXpm=\"0 0 2 1\",\n\"1 c #FF0000\",\n\"2 c #FF0000\",\n[end]\n";
  }

  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = map2fig(o,cnv);
    fig_make_label(ret, o.name, "4 0 0 55 -1 18 8 0.0 4");
    return ret;
  }

};

#endif
