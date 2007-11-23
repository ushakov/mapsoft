#ifndef ZN_L_H
#define ZN_L_H

// линейный условный знак на карте
// все прочие линейные знаки - его потомки

#include "zn.h"

class zn_l : public zn{
  public:
  zn_l(const std::string & _style = ""){
    name  = "неизвестный линейный знак";
    descr = "Прародитель всех линейных знаков!";
    base_fig.thickness = 3;
    base_fig.depth = 31;
    base_mp.Class = "POLYLINE";
    style = _style;
  }
};

class zn_l_horiz : public zn_l{
  public:
  zn_l_horiz(const std::string & _style = ""){
    name  = "горизонталь";
    descr = "";
    base_fig.pen_color = 26;
    base_fig.depth     = 90;
    base_fig.thickness = 1;
    base_mp.Type = 0x21;
    pfig_smooth = 0.3;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x21\nstring1=0x4"+name+"\nLineWidth=1\nXpm=\"0 0 1 1\"\n\"1 c #BD0000\",\n[end]\n";
  }
  virtual void fig_add_colors(fig::fig_world & W){
    if (style == "hr") base_fig.pen_color = zn::fig_add_color(W, 0xd0b090);
  }
};

class zn_l_thoriz : public zn_l_horiz{
  public:
  zn_l_thoriz(const std::string & _style = ""){
    name  = "толстая горизонталь";
    descr = "";
    base_fig.thickness = 2;
    base_mp.Type       = 0x22;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x22\nstring1=0x4"+name+"\nLineWidth=1\nXpm=\"0 0 1 1\"\n\"1 c #BD0000\",\n[end]\n";
  }
};

class zn_l_dhoriz : public zn_l_horiz{
  public:
  zn_l_dhoriz(const std::string & _style = ""){
    name  = "штриховая горизонталь";
    descr = "";
    base_fig.line_style= 1;
    base_mp.Type       = 0x20;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x22\nstring1=0x4"+name+"\nXpm=\"32 1 2 1\"\n"+
     "\"* c #BD0000\",\n\"  c none\",\"************    ************    \",\n[end]\n";

  }
};

class zn_l_kan : public zn_l_horiz{
  public:
  zn_l_kan(const std::string & _style = ""){
    name  = "сухая канава";
    descr = "";
    base_fig.line_style = 2;
    base_fig.cap_style  = 2;
    base_fig.style_val  = 2;
    base_mp.Type       = 0x2b;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\n"+
     "Type=0x2b\n"+
     "string1=0x4,"+name+"\n"+
     "Xpm=\"32 2 2 1\"\n"+
     "\"* c #BD0000\",\n"+
     "\"  c none\",\n"+
     "\"*****   *****   *****   *****   \",\n"+
     "\"*****   *****   *****   *****   \",\n"+
     "[end]\n";
  }
  virtual void fig_add_colors(fig::fig_world & W){}
};


class zn_l_ovrag : public zn_l{
  public:
  zn_l_ovrag(const std::string & _style = ""){
    name  = "овраг";
    descr = "";
    base_fig.pen_color = 25;
    base_fig.depth     = 89;
    base_fig.thickness = 2;
    base_mp.Type = 0x25;
    pfig_smooth = 0.3;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x25\nstring1=0x4"+name+"\nLineWidth=2\nXpm=\"0 0 1 1\"\n\"1 c #BD0000\",\n[end]\n";
  }
};

class zn_l_hreb : public zn_l{
  public:
  zn_l_hreb(const std::string & _style = ""){
    name  = "хребет";
    descr = "";
    base_fig.pen_color = 24;
    base_fig.depth     = 89;
    base_fig.thickness = 2;
    base_mp.Type = 0x0C;
    pfig_smooth = 0.3;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x0c\nstring1=0x4"+name+"\nLineWidth=3\nXpm=\"0 0 1 1\"\n\"1 c #BD0000\",\n[end]\n";
  }
};

class zn_l_cnt : public zn_l{
  public:
  zn_l_cnt(const std::string & _style = ""){
    name  = "контур растительности";
    descr = "";
    base_fig.pen_color = 12;
    base_fig.depth     = 87;
    base_fig.thickness = 1;
    base_mp.Type = 0x23;
    pfig_smooth = 0.3;
    base_fig.line_style = 2;
    base_fig.cap_style  = 2;
    base_fig.style_val  = 2;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\n"+
     "Type=0x23\n"+
     "string1=0x4,"+name+"\n"+
     "Xpm=\"32 2 2 1\"\n"+
     "\"* c #009500\",\n"+
     "\"  c none\",\n"+
     "\"**  **  **  **  **  **  **  **  \",\n"+
     "\"**  **  **  **  **  **  **  **  \",\n"+
     "[end]\n";
  }
};

class zn_l_reka3 : public zn_l{
  public:
  zn_l_reka3(const std::string & _style = ""){
    name  = "река-3";
    descr = "";
    base_fig.depth     = 86;
    base_fig.thickness = 3;
    base_mp.Type = 0x1f;
    pfig_smooth = 0.3;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x5066FF);
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x1f\nstring1=0x4"+name+"\nLineWidth=4\nXpm=\"0 0 1 1\"\n\"1 c #0000FF\",\n[end]\n";
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    if (ret.size()==1) {
      ret.push_back(*ret.begin());
      ret.begin()->pen_color=3;
      ret.begin()->thickness=1;
      ret.begin()->depth=84;
    }
    return ret;
  }
};

class zn_l_reka2 : public zn_l{
  public:
  zn_l_reka2(const std::string & _style = ""){
    name  = "река-2";
    descr = "";
    base_fig.depth     = 86;
    base_fig.thickness = 2;
    base_mp.Type = 0x18;
    pfig_smooth = 0.3;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x5066FF);
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x18\nstring1=0x4"+name+"\nLineWidth=3\nXpm=\"0 0 1 1\"\n\"1 c #0000FF\",\n[end]\n";
  }
};

class zn_l_reka1 : public zn_l{
  public:
  zn_l_reka1(const std::string & _style = ""){
    name  = "река-1";
    descr = "";
    base_fig.depth     = 86;
    base_fig.thickness = 1;
    base_mp.Type = 0x15;
    pfig_smooth = 0.3;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x5066FF);
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x15\nstring1=0x4"+name+"\nLineWidth=2\nXpm=\"0 0 1 1\"\n\"1 c #0000FF\",\n[end]\n";
  }
};

class zn_l_rekap : public zn_l{
  public:
  zn_l_rekap(const std::string & _style = ""){
    name  = "пересыхающая река";
    descr = "";
    base_fig.depth      = 86;
    base_fig.line_style = 1;
    base_fig.thickness  = 1;
    base_mp.Type = 0x26;
    pfig_smooth = 0.3;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x5066FF);
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\n"+
     "Type=0x26\n"+
     "string1=0x4,Ручей\n"+
     "Xpm=\"32 2 2 1\"\n"+
     "\"* c #0000FF\",\n"+
     "\"  c none\",\n"+
     "\"************    ************    \",\n"+
     "\"************    ************    \",\n"+
     "[end]\n";
  }
};

class zn_l_zd : public zn_l{
  public:
  zn_l_zd(const std::string & _style = ""){
    name  = "железная дорога";
    descr = "";
    base_fig.pen_color = 0;
    base_fig.depth     = 80;
    base_fig.thickness = 4;
    base_fig.cap_style = 0;
    base_mp.Type = 0x27;
    pfig_smooth = 0.3;
    style = _style;
  }
 
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x27\nUseOrientation=N\nANTIALIAS=N\nstring2=0x15"+name+"\n"+
     "Xpm=\"32 3 4 1\"\n"+
     "\"# c #000000\"\n"+
     "\"$ c #ffffff\"\n"+
     "\"3 c #000000\"\n"+
     "\"4 c #ffffff\"\n"+
     "\"################################\"\n"+
     "\"$$$########$$$$$$$$########$$$$$\"\n"+
     "\"################################\"\n"+
     "[end]\n";
  }
};

class zn_l_aasf : public zn_l{
  public:
  zn_l_aasf(const std::string & _style = ""){
    name  = "автомагистраль";
    descr = "";
    base_fig.depth     = 80;
    base_fig.thickness = 7;
    base_fig.cap_style = 0;
    base_mp.Type = 0x1;
    pfig_smooth = 0.3;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x404040);
  }

  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    if (ret.size()==1) {
      ret.begin()->pen_color=0;
      ret.push_back(*ret.begin());
      ret.begin()->pen_color=27;
      ret.begin()->thickness=5;
      ret.begin()->depth=79;
      ret.push_back(*ret.begin());
      ret.begin()->pen_color=0;
      ret.begin()->thickness=1;
      ret.begin()->depth=78;
    }
    return ret;
  }

  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x1\nstring1=0x4"+name+"\nLineWidth=3\nBorderWidth=1\nXpm=\"0 0 1 1\"\n\"1 c #FF0000\",\n2 c #000000\",\n[end]\n";
  }
};



class zn_l_asf : public zn_l{
  public:
  zn_l_asf(const std::string & _style = ""){
    name  = "асфальтовая дорога";
    descr = "Так можно обозначать и главные грунтовые трассы (типа дороги через пер.Анзоб :))";
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
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    if (ret.size()==1) {
      ret.begin()->pen_color=0;
      ret.push_back(*ret.begin());
      ret.begin()->pen_color=27;
      ret.begin()->thickness=2;
      ret.begin()->depth=79;
    }
    return ret;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x2\nstring1=0x4"+name+"\nLineWidth=2\nBorderWidth=1\nXpm=\"0 0 1 1\"\n\"1 c #FF0000\",\n2 c #000000\",\n[end]\n";
  }
};


class zn_l_greid : public zn_l{
  public:
  zn_l_greid(const std::string & _style = ""){
    name  = "проезжий грейдер";
    descr = "Проезжий грейдер - профилированная дорога, проезжая на машине. Так можно обозначать и побочные асфальты рядом с главным";
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
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    if (ret.size()==1) {
      ret.begin()->pen_color=0;
      ret.push_back(*ret.begin());
      ret.begin()->pen_color=7;
      ret.begin()->thickness=1;
      ret.begin()->depth=79;
    }
    return ret;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x4\nstring1=0x4"+name+"\nLineWidth=1\nBorderWidth=1\nXpm=\"0 0 1 1\"\n\"1 c #FFFFFF\",\n2 c #000000\",\n[end]\n";
  }
};

class zn_l_ngreid : public zn_l{
  public:
  zn_l_ngreid(const std::string & _style = ""){
    name  = "непроезжий грейдер";
    descr = "заросшая профилированная дорога";
    base_fig.depth      = 80;
    base_fig.line_style = 1;
    base_fig.thickness  = 3;
    base_fig.cap_style  = 0;
    base_mp.Type = 0x07;
    pfig_smooth  = 0.3;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x404040);
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    if (ret.size()==1) {
      ret.begin()->pen_color=0;
      ret.push_back(*ret.begin());
      ret.begin()->pen_color=7;
      ret.begin()->thickness=1;
      ret.begin()->depth=79;
    }
    return ret;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
      "[_line]\n"+
      "Type=0x7\n"+
      "UseOrientation=N\n"+
      "ANTIALIAS=N\n"+
      "string2=0x15,"+name+"\n"+
      "Xpm=\"32 3 4 1\"\n"+
      "\"# c #000000\",\n"+
      "\"$ c #FFFFFF\",\n"+
      "\"3 c #000000\",\n"+
      "\"4 c #FFFFFF\",\n"+
      "\"############$$$$############$$$$\",\n"+
      "\"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\",\n"+
      "\"############$$$$############$$$$\",\n"+
      "[end]\n";
  }
};

class zn_l_grunt : public zn_l{
  public:
  zn_l_grunt(const std::string & _style = ""){
    name  = "проезжая грунтовка";
    descr = "";
    base_fig.pen_color = 0;
    base_fig.depth     = 80;
    base_fig.thickness = 1;
    base_fig.cap_style = 0;
    base_mp.Type = 0x06;
    pfig_smooth  = 0.3;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x6\nstring1=0x4"+name+"\nLineWidth=2\nXpm=\"0 0 1 1\"\n\"1 c #000000\",\n[end]\n";
  }
};

class zn_l_ngrunt : public zn_l{
  public:
  zn_l_ngrunt(const std::string & _style = ""){
    name  = "непроезжая грунтовка";
    descr = "";
    base_fig.depth      = 80;
    base_fig.thickness  = 1;
    base_fig.cap_style  = 0;
    base_mp.Type = 0x0a;
    pfig_smooth  = 0.3;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x404040);
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    if (ret.size()==1) {
      ret.begin()->pen_color=0;
      ret.begin()->line_style = 1;
    }
    return ret;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\n"+
     "Type=0x0a\n"+
     "string1=0x4,"+name+"\n"+
     "Xpm=\"32 2 2 1\"\n"+
     "\"* c #000000\",\n"+
     "\"  c none\",\n"+
     "\"************    ************    \",\n"+
     "\"************    ************    \",\n"+
     "[end]\n";
  }
};

class zn_l_pros1 : public zn_l{
  public:
  zn_l_pros1(const std::string & _style = ""){
    name  = "просека";
    descr = "";
    base_fig.pen_color  = 0;
    base_fig.depth      = 80;
    base_fig.thickness  = 1;
    base_fig.cap_style  = 0;
    base_fig.line_style  = 1;
    base_mp.Type = 0x16;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
      "[_line]\n"+
      "Type=0x16\n"+
      "string1=0x4,"+name+"\n"+
      "Xpm=\"32 2 2 1\"\n"+
      "\"* c #000000\",\n"+
      "\"  c none\",\n"+
      "\"*****   *****   *****   *****   \",\n"+
      "\"*****   *****   *****   *****   \",\n"+
      "[end]\n";
  }
};

class zn_l_pros2 : public zn_l{
  public:
  zn_l_pros2(const std::string & _style = ""){
    name  = "широкая просека";
    descr = "";
    base_fig.pen_color  = 0;
    base_fig.depth      = 80;
    base_fig.thickness  = 2;
    base_fig.cap_style  = 0;
    base_fig.line_style  = 1;
    base_mp.Type = 0x1c;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
      "[_line]\n"+
      "Type=0x1c\n"+
      "string1=0x4,"+name+"\n"+
      "Xpm=\"32 3 2 1\"\n"+
      "\"* c #000000\",\n"+
      "\"  c none\",\n"+
      "\"************    ************    \",\n"+
      "\"************    ************    \",\n"+
      "\"************    ************    \",\n"+
      "[end]\n";
  }
};

class zn_l_trop : public zn_l{
  public:
  zn_l_trop(const std::string & _style = ""){
    name  = "тропа";
    descr = "";
    base_fig.pen_color  = 0;
    base_fig.depth      = 80;
    base_fig.thickness  = 1;
    base_fig.cap_style  = 2;
    base_fig.line_style = 2;
    base_fig.style_val  = 2;
    base_mp.Type = 0x2a;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
      "[_line]\n"+
      "Type=0x2a\n"+
      "string1=0x4,"+name+"\n"+
      "Xpm=\"32 2 2 1\"\n"+
      "\"* c #000000\",\n"+
      "\"  c none\",\n"+
      "\"*** *** *** *** *** *** *** *** \",\n"+
      "\"*** *** *** *** *** *** *** *** \",\n"+
      "[end]\n";
  }
};

class zn_l_dom : public zn_l{
  public:
  zn_l_dom(const std::string & _style = ""){
    name  = "дом";
    descr = "";
    base_fig.pen_color = 0;
    base_fig.depth     = 81;
    base_fig.thickness = 3;
    base_fig.cap_style = 0;
    base_mp.Type = 0x05;
    style = _style;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
     "[_line]\nType=0x3\nstring1=0x4"+name+"\nLineWidth=4\nXpm=\"0 0 1 1\"\n\"1 c #000000\",\n[end]\n";
  }
};

class zn_l_lep : public zn_l{
  public:
  zn_l_lep(const std::string & _style = ""){
    name  = "ЛЭП";
    descr = "";
    base_fig.depth     = 83;
    base_fig.thickness = 3;
    base_fig.cap_style = 0;
    base_mp.Type = 0x29;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x888888);
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
      "[_line]\n"+
      "Type=0x29\n"+
      "string1=0x4,"+name+"\n"+
      "Xpm=\"32 5 1 1\"\n"+
      "\"* c #000000\",\n"+
      "\"        *             ***       \",\n"+
      "\"        *               ***     \",\n"+
      "\"********************************\",\n"+
      "\"        *               ***     \",\n"+
      "\"        *             ***       \",\n"+
      "[end]\n";
  }
};

class zn_l_leps : public zn_l{
  public:
  zn_l_leps(const std::string & _style = ""){
    name  = "маленькая ЛЭП";
    descr = "";
    base_fig.depth     = 83;
    base_fig.thickness = 2;
    base_fig.cap_style = 0;
    base_mp.Type = 0x1A;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x888888);
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
      "[_line]\n"+
      "Type=0x1A\n"+
      "string1=0x4,"+name+"\n"+
      "Xpm=\"32 5 1 1\"\n"+
      "\"* c #959595\",\n"+
      "\"        *             ***       \",\n"+
      "\"        *               ***     \",\n"+
      "\"********************************\",\n"+
      "\"        *               ***     \",\n"+
      "\"        *             ***       \",\n"+
      "[end]\n";
  }

};

class zn_l_gaz : public zn_l{
  public:
  zn_l_gaz(const std::string & _style = ""){
    name  = "Газопровод";
    descr = "";
    base_fig.depth     = 83;
    base_fig.thickness = 3;
    base_fig.cap_style = 0;
    base_fig.line_style = 1;
    base_mp.Type = 0x28;
    style = _style;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x888888);
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) +
      "[_line]\n"+
      "Type=0x28\n"+
      "string1=0x4,"+name+"\n"+
      "Xpm=\"32 5 1 1\"\n"+
      "\"* c #000000\",\n"+
      "\"              ***               \",\n"+
      "\"             *   *              \",\n"+
      "\"**************   ***************\",\n"+
      "\"             *   *              \",\n"+
      "\"              ***               \",\n"+
      "[end]\n";
  }

};





#endif
