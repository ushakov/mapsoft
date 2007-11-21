#ifndef ZN_A_H
#define ZN_A_H

// площадной условный знак на карте
// все прочие линейные знаки - его потомки

#include "zn.h"

const std::string typ_kldb_fill = std::string("")+
      "\"********************************\",\n"+
      "\"********************************\",\n"+
      "\"***** ****************** *******\",\n"+
      "\"***** ****************** *******\",\n"+
      "\"***     **************     *****\",\n"+
      "\"***** ****************** *******\",\n"+
      "\"***** ****************** *******\",\n"+
      "\"***** ****************** *******\",\n"+
      "\"***** ********* ******** *******\",\n"+
      "\"*************** ****************\",\n"+
      "\"*************     **************\",\n"+
      "\"*************** ****************\",\n"+
      "\"*************** ****************\",\n"+
      "\"*************** ****************\",\n"+
      "\"*************** ****************\",\n"+
      "\"********************************\",\n"+
      "\"********************************\",\n"+
      "\"***** ******************* ******\",\n"+
      "\"***** ******************* ******\",\n"+
      "\"***     ***************     ****\",\n"+
      "\"***** ******************* ******\",\n"+
      "\"***** ******************* ******\",\n"+
      "\"***** ******************* ******\",\n"+
      "\"***** ******************* ******\",\n"+
      "\"*************** ****************\",\n"+
      "\"*************** ****************\",\n"+
      "\"*************     **************\",\n"+
      "\"*************** ****************\",\n"+
      "\"*************** ****************\",\n"+
      "\"*************** ****************\",\n"+
      "\"*************** ****************\",\n"+
      "\"********************************\",\n";

const std::string typ_vyr_fill = std::string("")+
      "\"  ****    ****    ****    ****  \",\n"+
      "\" **  **  **  **  **  **  **  ** \",\n"+
      "\"**    ****    ****    ****    **\",\n"+
      "\"*      **      **      **      *\",\n"+
      "\"**    ****    ****    ****    **\",\n"+
      "\" **  **  **  **  **  **  **  ** \",\n"+
      "\"  ****    ****    ****    ****  \",\n"+
      "\"   **      **      **      **   \",\n"+
      "\"  ****    ****    ****    ****  \",\n"+
      "\" **  **  **  **  **  **  **  ** \",\n"+
      "\"**    ****    ****    ****    **\",\n"+
      "\"*      **      **      **      *\",\n"+
      "\"**    ****    ****    ****    **\",\n"+
      "\" **  **  **  **  **  **  **  ** \",\n"+
      "\"  ****    ****    ****    ****  \",\n"+
      "\"   **      **      **      **   \",\n"+
      "\"  ****    ****    ****    ****  \",\n"+
      "\" **  **  **  **  **  **  **  ** \",\n"+
      "\"**    ****    ****    ****    **\",\n"+
      "\"*      **      **      **      *\",\n"+
      "\"**    ****    ****    ****    **\",\n"+
      "\" **  **  **  **  **  **  **  ** \",\n"+
      "\"  ****    ****    ****    ****  \",\n"+
      "\"   **      **      **      **   \",\n"+
      "\"  ****    ****    ****    ****  \",\n"+
      "\" **  **  **  **  **  **  **  ** \",\n"+
      "\"**    ****    ****    ****    **\",\n"+
      "\"*      **      **      **      *\",\n"+
      "\"**    ****    ****    ****    **\",\n"+
      "\" **  **  **  **  **  **  **  ** \",\n"+
      "\"  ****    ****    ****    ****  \",\n"+
      "\"   **      **      **      **   \",\n";

const std::string typ_rles_fill = std::string("")+
      "\"*   *   *   *   *   *   *   *   \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"  *   *   *   *   *   *   *   * \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"*   *   *   *   *   *   *   *   \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"  *   *   *   *   *   *   *   * \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"*   *   *   *   *   *   *   *   \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"  *   *   *   *   *   *   *   * \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"*   *   *   *   *   *   *   *   \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"  *   *   *   *   *   *   *   * \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"*   *   *   *   *   *   *   *   \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"  *   *   *   *   *   *   *   * \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"*   *   *   *   *   *   *   *   \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"  *   *   *   *   *   *   *   * \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"*   *   *   *   *   *   *   *   \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"  *   *   *   *   *   *   *   * \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"*   *   *   *   *   *   *   *   \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n"+
      "\"  *   *   *   *   *   *   *   * \",\n"+
      "\" * * * * * * * * * * * * * * * *\",\n";

/****************************************************/
class zn_a : public zn{
  public:
  zn_a(const std::string & _style = ""){
    name  = "неизвестный площадной знак";
    descr = "Прародитель всех площадных знаков!";
    base_fig.type       = 2;
    base_fig.sub_type   = 3;
    base_fig.area_fill  = 20;
    base_fig.depth      = 32;
    base_mp.Class = "POLYGON";
    style = _style;
  }
};

/****************************************************/
class zn_a_der : public zn_a{
  public:
  zn_a_der(const std::string & _style = ""){
    name  = "деревня";
    descr = "Деревня, поселок, любая небольшая жилая территория";
    base_fig.pen_color   = 0;
    base_fig.fill_color  = 27;
    base_fig.depth       = 94;
    base_mp.Type         = 0xE;
    typ_order = 5;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
     "[_polygon]\nType=0xE\nstring1=0x4,"+name+"\nXpm=\"0 0 2 1\",\n\"1 c #FF0000\",\n\"2 c #FF0000\",\n[end]\n";
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = map2fig(o,cnv);
    fig_make_label(ret, o.name, "4 0 0 55 -1 18 8 0.0 4");
    return ret;
  }
};


/****************************************************/
class zn_a_gor : public zn_a{
  public:
  zn_a_gor(const std::string & _style = ""){
    name  = "город";
    descr = "Город, крупный поселок, большая жилая территория";
    base_fig.pen_color   = 0;
    base_fig.fill_color  = 27;
    base_fig.area_fill   = 15;
    base_fig.depth       = 94;
    base_mp.Type         = 0x1;
    typ_order = 5;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
     "[_polygon]\nType=0x1\nstring1=0x4,"+name+"\nXpm=\"0 0 2 1\",\n\"1 c #FF0000\",\n\"2 c #FF0000\",\n[end]\n";
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = map2fig(o,cnv);
    if (ret.size()==1){ret.begin()->area_fill=20;}
    fig_make_label(ret, o.name, "4 0 0 55 -1 18 8 0.0 4");
    return ret;
  }
};


/****************************************************/
class zn_a_zakr : public zn_a{
  public:
  zn_a_zakr(const std::string & _style = ""){
    name  = "закрытая территория";
    descr = "Промышленные, военные и т.п. территории";
    base_fig.pen_color   = 0;
    base_fig.fill_color  = 7;
    base_fig.area_fill   = 15;
    base_fig.depth       = 95;
    base_mp.Type         = 0x4;
    typ_order = 5;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
     "[_polygon]\nType=0x4\nstring1=0x4,"+name+"\nXpm=\"0 0 2 1\",\n\"1 c #D5D5D5\",\n\"2 c #D5D5D5\",\n[end]\n";
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = map2fig(o,cnv);
    fig_make_label(ret, o.name, "4 0 0 55 -1 3 8 0.0 4");
    return ret;
  }
};

/****************************************************/
class zn_a_dach : public zn_a{
  public:
  zn_a_dach(const std::string & _style = ""){
    name  = "дачи";
    descr = "дачи, сад.уч., д/о, п/л и т.п.";
    base_fig.pen_color   = 0;
    base_fig.area_fill   = 10;
    base_fig.depth       = 93;
    base_mp.Type         = 0x4E;
    typ_order = 6;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
     "[_polygon]\nType=0x4E\nstring1=0x4,"+name+"\nXpm=\"0 0 2 1\",\n\"1 c #00A500\",\n\"2 c #00A500\",\n[end]\n";
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = map2fig(o,cnv);
    fig_make_label(ret, o.name, "4 0 0 55 -1 3 8 0.0 4");
    return ret;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.fill_color = zn::fig_add_color(W, 0xAAFFAA);
  }
};

/****************************************************/
class zn_a_kldb : public zn_a{
  public:
  zn_a_kldb(const std::string & _style = ""){
    name  = "кладбище";
    descr = "кладбище";
    base_fig.pen_color   = 0;
    base_fig.area_fill   = 5;
    base_fig.depth       = 92;
    base_mp.Type         = 0x1A;
    typ_order = 6;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
      "[_polygon]\nType=0x1A\nstring1=0x4,"+name+
      "\nXpm=\"32 32 2 1\",\n\"  c #000000\",\n\"* c #00A500\",\n"+typ_kldb_fill+"[end]\n";
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = map2fig(o,cnv);
    if (ret.size()==1){
      ret.begin()->area_fill=10;
      int w1=23, w2=45;

      int minx=0x7FFFFFFF, maxx=-0x7FFFFFFF;
      int miny=0x7FFFFFFF, maxy=-0x7FFFFFFF;
      for (int j=0; j<ret.begin()->size(); j++){
        if ((*ret.begin())[j].x>maxx) maxx=(*ret.begin())[j].x;
        if ((*ret.begin())[j].x<minx) minx=(*ret.begin())[j].x;
        if ((*ret.begin())[j].y>maxy) maxy=(*ret.begin())[j].y;
        if ((*ret.begin())[j].y<miny) miny=(*ret.begin())[j].y;
      }
      if ((minx<maxx)&&(miny<maxy)){

        int x=(maxx+minx)/2, y=(maxy+miny)/2-w1/2;

        fig::fig_object o = fig::make_object("2 1 0 1 0 * 57 * * * * 0 * * * *");
        o.push_back(Point<int>(x-w1,y));
        o.push_back(Point<int>(x+w1,y));
        ret.push_back(o);
        o.clear();
        o.push_back(Point<int>(x,y-w1));
        o.push_back(Point<int>(x,y+w2));
        ret.push_back(o);
      }
    }
    fig_make_label(ret, o.name, "4 0 0 55 -1 3 8 0.0 4");
    return ret;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.fill_color = zn::fig_add_color(W, 0xAAFFAA);
  }
};

/****************************************************/

class zn_a_les2 : public zn_a{
  public:
  zn_a_les2(const std::string & _style = ""){
    name  = "лес";
    descr = "лес, нарисованный поверх поля, вырубок";
    base_fig.pen_color   = 12;
    base_fig.thickness   = 0;
    base_fig.depth       = 97;
    base_mp.Type         = 0x15;
    typ_order = 4;
    pfig_smooth = 0.3;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
     "[_polygon]\nType=0x15\nstring1=0x4,"+name+"\nXpm=\"0 0 2 1\",\n\"1 c #00FF00\",\n\"2 c #00FF00\",\n[end]\n";
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.fill_color = zn::fig_add_color(W, 0xAAFFAA);
  }
};

/****************************************************/

class zn_a_les : public zn_a{
  public:
  zn_a_les(const std::string & _style = ""){
    name  = "лес";
    descr = "лес (фон карты)";
    base_fig.pen_color   = 12;
    base_fig.thickness   = 0;
    base_fig.depth       = 100;
    base_mp.Type         = 0x16;
    typ_order = 1;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
     "[_polygon]\nType=0x16\nstring1=0x4,"+name+"\nXpm=\"0 0 2 1\",\n\"1 c #00FF00\",\n\"2 c #00FF00\",\n[end]\n";
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.fill_color = zn::fig_add_color(W, 0xAAFFAA);
  }
};

/****************************************************/

class zn_a_pole : public zn_a{
  public:
  zn_a_pole(const std::string & _style = ""){
    name  = "поле";
    descr = "поле";
    base_fig.pen_color   = 12;
    base_fig.fill_color  = 7;
    base_fig.thickness   = 0;
    base_fig.depth       = 99;
    base_mp.Type         = 0x52;
    typ_order = 2;
    pfig_smooth = 0.3;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
     "[_polygon]\nType=0x52\nstring1=0x4,"+name+"\nXpm=\"0 0 2 1\",\n\"1 c #FFFFFF\",\n\"2 c #FFFFFF\",\n[end]\n";
  }
};

/****************************************************/

class zn_a_ozer : public zn_a{
  public:
  zn_a_ozer(const std::string & _style = ""){
    name  = "озеро";
    descr = "озеро, небольшой водоем";
    base_fig.fill_color  = 3;
    base_fig.depth       = 85;
    base_mp.Type         = 0x29;
    typ_order = 7;
    pfig_smooth = 0.3;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
     "[_polygon]\nType=0x29\nstring1=0x4,"+name+"\nXpm=\"0 0 2 1\",\n\"1 c #0000FF\",\n\"2 c #0000FF\",\n[end]\n";
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x5066FF);
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    fig_make_label(ret, o.name, "4 0 0 55 -1 3 8 0.0 4");
    return ret;
  }
};

/****************************************************/

class zn_a_more : public zn_a{
  public:
  zn_a_more(const std::string & _style = ""){
    name  = "большой водоем";
    descr = "большой водоем";
    base_fig.fill_color  = 3;
    base_fig.area_fill   = 15;
    base_fig.depth       = 85;
    base_mp.Type         = 0x3B;
    typ_order = 7;
    pfig_smooth = 0.3;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
     "[_polygon]\nType=0x3B\nstring1=0x4,"+name+"\nXpm=\"0 0 2 1\",\n\"1 c #0000FF\",\n\"2 c #0000FF\",\n[end]\n";
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x5066FF);
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    if (ret.size()==1){ret.begin()->area_fill=20;}
    fig_make_label(ret, o.name, "4 0 0 55 -1 3 8 0.0 4");
    return ret;
  }
};

/****************************************************/

class zn_a_ostr : public zn_a{
  public:
  zn_a_ostr(const std::string & _style = ""){
    name  = "остров";
    descr = "остров";
    base_fig.fill_color  = 7;
    base_fig.depth       = 84;
    base_mp.Type         = 0x53;
    typ_order = 8;
    pfig_smooth = 0.3;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
     "[_polygon]\nType=0x53\nstring1=0x4,"+name+"\nXpm=\"0 0 2 1\",\n\"1 c #FFFFFF\",\n\"2 c #FFFFFF\",\n[end]\n";
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x5066FF);
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    if (ret.size()==1){ret.begin()->area_fill=20;}
    fig_make_label(ret, o.name, "4 0 0 55 -1 3 8 0.0 4");
    return ret;
  }
};

/****************************************************/
class zn_a_vyr1 : public zn_a{
  public:
  zn_a_vyr1(const std::string & _style = ""){
    name  = "свежая вырубка";
    descr = "свежая вырубка";
    base_fig.fill_color  = 7;
    base_fig.pen_color  = 12;
    base_fig.thickness   = 0;
    base_fig.area_fill  = 43;
    base_fig.depth       = 98;
    base_mp.Type         = 0x4F;
    typ_order = 3;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
      "[_polygon]\nType=0x4F\nstring1=0x4,"+name+"\n"+
      "Xpm=\"32 32 2 1\",\n\"  c #FFFFFF\",\n\"* c #00A500\",\n"+typ_vyr_fill+"[end]\n";
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    fig_make_label(ret, o.name, "4 0 0 55 -1 3 8 0.0 4");
    return ret;
  }
};

/****************************************************/
class zn_a_vyr2 : public zn_a{
  public:
  zn_a_vyr2(const std::string & _style = ""){
    name  = "старая вырубка";
    descr = "старая вырубка";
    base_fig.pen_color  = 12;
    base_fig.thickness   = 0;
    base_fig.area_fill  = 43;
    base_fig.depth       = 98;
    base_mp.Type         = 0x50;
    typ_order = 3;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
      "[_polygon]\nType=0x50\nstring1=0x4,"+name+"\n"+
      "Xpm=\"32 32 2 1\",\n\"  c #00FF00\",\n\"* c #00A500\",\n"+typ_vyr_fill+"[end]\n";
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    fig_make_label(ret, o.name, "4 0 0 55 -1 3 8 0.0 4");
    return ret;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.fill_color = zn::fig_add_color(W, 0xAAFFAA);
  }
};

/****************************************************/
class zn_a_rles : public zn_a{
  public:
  zn_a_rles(const std::string & _style = ""){
    name  = "редколесье";
    descr = "резколесье";
    base_fig.fill_color  = 7;
    base_fig.thickness   = 0;
    base_fig.area_fill  = 43;
    base_fig.depth       = 98;
    base_mp.Type         = 0x51;
    typ_order = 3;
    pfig_smooth = 0.3;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
      "[_polygon]\nType=0x51\nstring1=0x4,"+name+"\n"+
      "Xpm=\"32 32 2 1\",\n\"  c #FFFFFF\",\n\"* c #00FF00\",\n"+typ_rles_fill+"[end]\n";
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    fig_make_label(ret, o.name, "4 0 0 55 -1 3 8 0.0 4");
    return ret;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0xAAFFAA);
  }
};

/****************************************************/
class zn_a_ledn : public zn_a{
  public:
  zn_a_ledn(const std::string & _style = ""){
    name  = "ледник";
    descr = "ледник";
    base_fig.fill_color  = 7;
    base_fig.thickness   = 0;
    base_fig.area_fill   = 43;
    base_fig.depth       = 98;
    base_mp.Type         = 0x4D;
    typ_order = 3;
    pfig_smooth = 0.3;
  }
  virtual std::string get_typ(const std::string & style) const {
    return zn::get_typ(style) + 
      "[_polygon]\nType=0x4D\nstring1=0x4,"+name+"\n"+
      "Xpm=\"32 32 2 1\",\n\"  c #0000FF\",\n\"* c #00A500\",\n"+typ_vyr_fill+"[end]\n";
  }
  virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
    std::list<fig::fig_object> ret = zn::map2pfig(o,cnv);
    fig_make_label(ret, o.name, "4 0 0 55 -1 3 8 0.0 4");
    return ret;
  }
  virtual void fig_add_colors(fig::fig_world & W){
    base_fig.pen_color = zn::fig_add_color(W, 0x5066FF);
  }
};



#endif
