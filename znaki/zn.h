#ifndef ZN_H
#define ZN_H

// любой условный знак на карте
// все прочие знаки - его потомки

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "map_object.h"
#include "../geo_io/fig.h"
#include "../geo_io/mp.h"
#include "../geo_io/geo_convs.h"

class zn{
  public: // разные параметры. Потомкам может быть удобно менять эти параметры, но не менять сами процедуры вывода по умолчанию...
    std::string descr;
    int mp_type;
    double pfig_smooth; // насколько сглаживать углы в версии для печати? 0 - не сглаживать
    std::string style;
    fig::fig_object base_fig;
    mp::mp_object   base_mp;
    int typ_order;

 
    zn(const std::string & _style = ""){
      descr       = "неизвестный науке знак!";
      base_fig    = fig::make_object("2 1 0 1  4 6 3 -1 -1 0.000 1 1 0 0 0 *");
      base_mp     = mp::make_object("POI 0xFFFFFFFF 0 0");
      pfig_smooth = 0;
      typ_order   = 1;
      style       = _style;
    }

    // добавление дополнительных цветов
    virtual void fig_add_colors(fig::fig_world & W){}


    // добавить цвет C, если его еще нет
    int fig_add_color(fig::fig_world & W, int C){
      int last=0;
      for (std::map<int,int>::const_iterator i=W.colors.begin(); i!=W.colors.end(); i++){
        if (i->second == C) return i->first;
        if (last < i->first) last = i->first; 
      }
      W.colors[last+1] = C;
      return last+1;
    }

    // fig и mp "для редактирования"
    virtual std::list<fig::fig_object> map2fig(const map_object & o, convs::map2pt & cnv) const {
      std::list<fig::fig_object> ret;
      fig::fig_object obj = base_fig;

      obj.comment.push_back(o.name);
      for (int i=0; i<o.comm.size(); i++) obj.comment.push_back(o.comm[i]);

      std::vector<g_point> pts = cnv.line_bck(o);
      for (int i=0; i<pts.size(); i++) obj.push_back(pts[i]);

      if (obj.size()!=0) ret.push_back(obj);
      return ret;
    }

    virtual std::list<mp::mp_object>   map2mp(const map_object & o) const {
      std::list<mp::mp_object> ret;
      mp::mp_object obj=base_mp;
      obj.Label = o.name;
      obj.Comment = o.comm;
      obj.Class = (o.size()==1)?"POI":"POLYLINE";
      for (int i=0; i<o.size(); i++) obj.push_back(o[i]);
      if (obj.size()!=0) ret.push_back(obj);
      return ret;
    }

    // fig и mp "для печати"
    // по умолчанию -- такие же, как и для редактирования
    virtual std::list<fig::fig_object>  map2pfig(const map_object & o, convs::map2pt & cnv) const {
      std::list<fig::fig_object> ret = map2fig(o,cnv); 
      std::list<fig::fig_object>::iterator l0 = ret.begin();

      int fs = l0->size();
      // сгладим линии, если надо
      if ((pfig_smooth!=0) && (ret.size()>0) && (fs>2)){

        if (l0->type==2){
          if (l0->sub_type<=1) l0->sub_type=4; else l0->sub_type=5;
        }
        if (l0->type==3){
          l0->sub_type = (l0->sub_type%2)+4;
        }
        if ((*l0)[0]==(*l0)[fs-1]){
          l0->erase(l0->begin()+fs-1);
          fs=l0->size();
        }

        l0->type=3;
        l0->f.clear();

        for (int j=0; j< fs; j++) l0->f.push_back(0.3);
        if (l0->sub_type==4){
          l0->f[0]=0;
          l0->f[fs-1]=0;
        }

      }
      return ret;
    }
    virtual std::list<mp::mp_object>    map2pmp(const map_object & o) const {
      return map2mp(o);
    }

    // проверка, что объект fig или mp соответствует данному типу и его можно преобразовать в map_object
    virtual bool test_fig(const fig::fig_object & o, const fig::fig_world & W) const {

      // для неизвестных объектов
      if (base_mp.Type==0xFFFFFFFF) {
        if (base_mp.Class=="POI"){
          return ((o.type==2) && (o.size()==1));
        }
        if (base_mp.Class=="POLYLINE"){
          if (o.size()<2) return false;
          if ((o.type==2) && (o.sub_type!=1)) return false;
          if ((o.type==3) && (o.sub_type%2!=0)) return false;
          return true;
        }
        if (base_mp.Class=="POLYGON"){
          if (o.size()<2) return false;
          if ((o.type==2) && (o.sub_type<2)) return false;
          if ((o.type==3) && (o.sub_type%2==0)) return false;
          return true;
        }
      }

      // по умолчанию должны совпасть глубина, толщина,
      if ((o.type!=2) && (o.type==3)) return false;
      if ((o.depth     != base_fig.depth) ||
          (o.thickness != base_fig.thickness)) return false;
      // для линий толщины не 0 - еще и цвет и тип линии
      int c1 = o.pen_color, c2 = base_fig.pen_color;
      if (W.colors.find(c1)!=W.colors.end()) c1 = W.colors.find(c1)->second;
      if (W.colors.find(c2)!=W.colors.end()) c2 = W.colors.find(c2)->second;
      if ((o.thickness  != 0 ) && 
          ((c1 != c2) || (o.line_style != base_fig.line_style))) return false;
      // для многоугольников - еще и цвет и вид заливки
      bool closed1 = false, closed2 = false;
      if ((o.type==2)&&(o.sub_type>1))       closed1 = true;
      if ((o.type==3)&&(o.sub_type %2 == 1)) closed1 = true;
      if ((base_fig.type==2)&&(base_fig.sub_type>1))       closed2 = true;
      if ((base_fig.type==3)&&(base_fig.sub_type %2 == 1)) closed2 = true;
      if (closed1!=closed2) return false;
      int af1 = o.area_fill; 
      int af2 = base_fig.area_fill; 
      int fc1 = o.fill_color; 
      int fc2 = base_fig.fill_color; 
      // белая заливка бывает двух видов
      if ((fc1!=7)&&(af1==40)) {fc1=7; af1=20;}
      if ((fc2!=7)&&(af2==40)) {fc2=7; af2=20;}
      if (closed1 && ((fc1 != fc2) || (af1 != af2))) return false;
      return true;
    }

    virtual bool test_mp(const mp::mp_object & o) const {
      // для неизвестных объектов
      if (base_mp.Type==0xFFFFFFFF) {
        return (base_mp.Class==o.Class);
      }

      // по умолчанию должен совпасть тип объекта
      return ((o.Class == base_mp.Class) && (o.Type == base_mp.Type));
    }


    // чтение fig и mp
    virtual map_object fig2map(const fig::fig_object & o, convs::map2pt & cnv) const{
//      std::cerr << "fig2map: " << descr << "\n";
      map_object ret(this);
      
      std::vector<g_point> pts = cnv.line_bck(o);

      for (int i=0; i<pts.size(); i++) ret.push_back(pts[i]);
      if (o.comment.size()>0) ret.name = *o.comment.begin();
      for (int i=1; i<o.comment.size(); i++) ret.comm.push_back(o.comment[i]);
      return ret;
    }

    virtual map_object mp2map(const mp::mp_object & o) const{
//      std::cerr << "mp2map: " << descr << " " << o.Type << "\n";
      map_object ret(this);
      for (int i=0; i<o.size(); i++) ret.push_back(o[i]);
      ret.name = o.Label;
      ret.comm = o.Comment;
      return ret;
    }

    virtual std::string get_typ(const std::string & style) const{
      std::ostringstream s;
      if (base_mp.Type==0xFFFFFFFF) return std::string();
      s << "; " << base_mp.Class << std::setbase(16) 
        << " 0x" << base_mp.Type << " " << descr << "\n";
      return s.str();
    }

};

#endif
