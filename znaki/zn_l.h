#ifndef ZN_L_H
#define ZN_L_H

// линейный условный знак на карте
// все прочие линейные знаки - его потомки

#include "zn.h"

class zn_l : public zn{
  public:
  zn_l(const std::string & _style = ""){
    descr = "неизвестный науке линейный знак!";
    base_fig.depth = 3;
    base_mp.Class = "POLYLINE";
    style = _style;
  }
};

class zn_l_horiz : public zn_l{
  public:
  zn_l_horiz(const std::string & _style = ""){
    descr = "горизонталь";
    base_fig.pen_color = 26;
    base_fig.depth     = 90;
    base_mp.Type = 0x21;
    pfig_smooth = 0.3;
    style = _style;
  }
  // бергштрихи и подписи - в навеску??
  // толстые и пунктирные - в завис.от комментария?
};

class zn_l_asf : public zn_l{
  public:
  zn_l_asf(const std::string & _style = ""){
    descr = "асфальтовая дорога";
    base_fig.depth     = 80;
    base_fig.thickness = 4;
    base_fig.cap_style = 0;
    base_mp.Type = 0x02;
    pfig_smooth  = 0.3;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x404040);
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = map2fig(o,cnv);
    if (ret.size()==1) {
      ret.begin()->pen_color=0;
      ret.push_back(*ret.begin());
      ret.begin()->pen_color=27;
      ret.begin()->thickness=2;
      ret.begin()->depth=79;
    }
    return ret;
  }
};

class zn_l_greid : public zn_l{
  public:
  zn_l_greid(const std::string & _style = ""){
    descr = "проезжий грейдер";
    base_fig.depth     = 80;
    base_fig.thickness = 3;
    base_fig.cap_style = 0;
    base_mp.Type = 0x04;
    pfig_smooth  = 0.3;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x404040);
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = map2fig(o,cnv);
    if (ret.size()==1) {
      ret.begin()->pen_color=0;
      ret.push_back(*ret.begin());
      ret.begin()->pen_color=7;
      ret.begin()->thickness=1;
      ret.begin()->depth=79;
    }
    return ret;
  }
};


#endif
