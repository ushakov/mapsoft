#ifndef ZN_P_H
#define ZN_P_H

// точечный условный знак на карте
// все прочие точечные знаки - его потомки

#include "zn.h"
#include "utils.h"

class zn_p : public zn{
  public:
  zn_p(const std::string & _style = ""){
    descr = "неизвестный науке точечный знак!";
    base_fig.thickness = 5;
    base_fig.depth     = 2;
    base_mp.Class = "POI";
    style = _style;
  }
};

class zn_p_cerkov : public zn_p{

  int d1; // размер крестика

  public:
  zn_p_cerkov(const std::string & _style = ""){
    descr = "церковь";
    base_fig.pen_color = 11;
    base_fig.thickness = 1;
    base_fig.depth     = 157;
    base_mp.Type       = 0x2C0B;
    style = _style;
    d1 = int(0.2 * fig::cm2fig);
  }
  virtual std::list<fig::fig_object> map2fig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn_p::map2fig(o, cnv);
    if ((ret.size() != 1)||(ret.begin()->size() != 1)) return ret;

    fig::fig_object o1 = *ret.begin();
    int x = o1[0].x;
    int y = o1[0].y;
    o1.pen_color = 0;
    o1.thickness = 2;
    o1.cap_style = 0;
    o1.depth     = 57;
    o1.comment.clear();
    o1.clear();
    o1.push_back(Point<int>(x-d1/2, y));
    o1.push_back(Point<int>(x+d1/2, y));
    ret.push_back(o1);

    o1.clear();
    o1.push_back(Point<int>(x, y-d1/2));
    o1.push_back(Point<int>(x, y+d1/2));
    ret.push_back(o1);

    fig_make_comp(ret);
    return ret;
  }

};

#endif
