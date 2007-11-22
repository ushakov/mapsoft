#ifndef ZN_P_H
#define ZN_P_H

// точечный условный знак на карте
// все прочие точечные знаки - его потомки

#include "zn.h"
#include "utils.h"

class zn_p : public zn{
  public:
  zn_p(const std::string & _style = ""){
    name =  "неизвестный точечный знак";
    descr = "Прародитель всех точечных знаков!";
    base_fig.thickness = 5;
    base_fig.depth     = 30;
    base_mp.Class = "POI";
    style = _style;
  }
};



class zn_p_cerkov : public zn_p{

  int d1; // размер крестика

  public:
  zn_p_cerkov(const std::string & _style = ""){
    name = "церковь";
    descr = "";
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

class zn_p_avt : public zn_p{

  public:
  zn_p_avt(const std::string & _style = ""){
    name = "автобусная остановка";
    descr = "";
    base_fig.pen_color = 3;
    base_fig.thickness = 7;
    base_fig.cap_style = 1;
    base_fig.depth     = 157;
    base_mp.Type       = 0x2F08;
    style = _style;
  }
  virtual std::list<fig::fig_object> map2fig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn_p::map2fig(o, cnv);
    if ((ret.size() != 1)||(ret.begin()->size() != 1)) return ret;

    fig::fig_object o1;

    o1 = fig::make_object("1 3 0 1 4 7 57 -1 20 2.000 1 0.000 * * 55 55 * * * *");
    o1.push_back((*ret.begin())[0]);
    o1.center_x = o1.start_x = o1.end_x = o1[0].x;
    o1.center_y = o1.start_y = o1.end_y = o1[0].y;
    ret.push_back(o1);

    o1 = fig::make_object("4 1 4 55 -1 18 6 0.0000 4");
    o1.push_back((*ret.begin())[0]);
    o1[0].y+=40;
    o1.text = "A";
    ret.push_back(o1);

    fig_make_comp(ret);
    return ret;
  }
};

class zn_p_vys : public zn_p{

  public:
  zn_p_vys(const std::string & _style = ""){
    name = "отметка высоты";
    descr = "";
    base_fig.pen_color = 24;
    base_fig.thickness = 4;
    base_fig.cap_style = 1;
    base_fig.depth     = 57;
    base_mp.Type       = 0x1100;
    style = _style;
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = map2fig(o,cnv);
    fig_make_label(ret, o.name, "4 0 0 55 -1 18 7 0.0 4");
    return ret;
  }
};



#endif
