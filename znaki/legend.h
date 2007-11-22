#ifndef LEGEND_H
#define LEGEND_H

// работа со всеми условными знаками

#include "zn.h"
#include "zn_p.h"
#include "zn_l.h"
#include "zn_a.h"

class legend{
  std::vector<zn*> Z;
  std::string style;

  public:

  legend(std::string _style = "") : style(_style) {
    // Сюда должны быть добавлены все знаки, которые есть!
    // Порядок м.б. важен. Знаки перебираются до первого совпадения

    Z.push_back(new zn_l_horiz(style));  // горизонталь
    Z.push_back(new zn_l_thoriz(style)); // 
    Z.push_back(new zn_l_dhoriz(style)); // 
    Z.push_back(new zn_l_ovrag(style));  // 
    Z.push_back(new zn_l_hreb(style));   // 
    Z.push_back(new zn_l_reka3(style));  // 
    Z.push_back(new zn_l_reka2(style));  // 
    Z.push_back(new zn_l_reka1(style));  // 
    Z.push_back(new zn_l_rekap(style));  // 
    Z.push_back(new zn_l_zd(style));     // ж/д
    Z.push_back(new zn_l_aasf(style));   // 
    Z.push_back(new zn_l_asf(style));    // асфальтовая дорога
    Z.push_back(new zn_l_greid(style));  // грейдер
    Z.push_back(new zn_l_ngreid(style)); // непроезжий грейдер
    Z.push_back(new zn_l_grunt(style));  // проезжая грунтовка
    Z.push_back(new zn_l_ngrunt(style)); // непроезжая грунтовка
    Z.push_back(new zn_l_pros1(style));  // просека
    Z.push_back(new zn_l_pros2(style));  // широкая просека
    Z.push_back(new zn_l_trop(style));   // тропа
    Z.push_back(new zn_l_dom(style));    // отдельные строения

    Z.push_back(new zn_a_der(style));    // 
    Z.push_back(new zn_a_gor(style));    // 
    Z.push_back(new zn_a_zakr(style));   // 
    Z.push_back(new zn_a_dach(style));   // 
    Z.push_back(new zn_a_kldb(style));   // 
    Z.push_back(new zn_a_les(style));    // 
    Z.push_back(new zn_a_les2(style));   // 
    Z.push_back(new zn_a_pole(style));   // 
    Z.push_back(new zn_a_ozer(style));   // 
    Z.push_back(new zn_a_more(style));   // 
    Z.push_back(new zn_a_ostr(style));   // 
    Z.push_back(new zn_a_vyr1(style));   // 
    Z.push_back(new zn_a_vyr2(style));   // 
    Z.push_back(new zn_a_rles(style));   // 
    Z.push_back(new zn_a_ledn(style));   // 

    Z.push_back(new zn_p_cerkov(style));
    Z.push_back(new zn_p_avt(style));
    Z.push_back(new zn_p_vys(style));

    Z.push_back(new zn_p(style));
    Z.push_back(new zn_l(style));
    Z.push_back(new zn_a(style));
    Z.push_back(new zn(style));
  }
  ~legend(){
    for (int i = 0; i<Z.size(); i++){
      delete Z[i];
    }
  }

  // добавление дополнительных цветов!
  void fig_add_colors(fig::fig_world & W){
    for (int i = 0; i<Z.size(); i++){
      Z[i]->fig_add_colors(W);
    }
  }

  map_object mp2map(const mp::mp_object & o) const{
    for (int i = 0; i<Z.size(); i++){
      if (Z[i]->test_mp(o)) return Z[i]->mp2map(o);
    }
    return map_object();
  }

  map_object fig2map(const fig::fig_object & o, const fig::fig_world & W, convs::map2pt & cnv) const{
    for (int i = 0; i<Z.size(); i++){
      if (Z[i]->test_fig(o, W)) return Z[i]->fig2map(o,cnv);
    }
    return map_object();
  }

  // typ-файл
  void print_typ(std::ostream & s) const{

    s << "[_drawOrder]\n";
    for (int i = 0; i<Z.size(); i++){
      if (Z[i]->base_mp.Class != "POLYGON") continue;
      if (Z[i]->base_mp.Type == 0xFFFFFFFF) continue;
      s << "Type=" << std::setbase(16)
        << " 0x" << Z[i]->base_mp.Type << "," 
        << Z[i]->typ_order << "\n";
    }
    s << "[end]\n";

    for (int i = 0; i<Z.size(); i++){
      if (Z[i]->base_mp.Type==0xFFFFFFFF) continue;
      s << "; " << Z[i]->base_mp.Class << std::setbase(16)
        << " 0x" << Z[i]->base_mp.Type << " " << Z[i]->name << "\n"
        << "; " << Z[i]->descr << "\n"
        << Z[i]->get_typ(style) << "\n";
    }
  }
  // fig-файл и библиотека
  void print_figlib(std::string prefix){
    int ll=200; // характерный размер в единицах xfig

    g_map map; // простое преобразование из fig в lonlat
    map.push_back(g_refpoint(0,0,0,0));
    map.push_back(g_refpoint(1,0,ll,0));
    map.push_back(g_refpoint(0,1,0,ll));
    map.border.push_back(g_point(0,0));
    map.border.push_back(g_point(0,0));
    map.border.push_back(g_point(0,0));
    map.map_proj = Proj("lonlat");
    convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"), Options());

    // тестовые объекты - точка, линия, прямоугольник
    map_object o1pt, o2pt, o4pt, *o;       
    o1pt.push_back(g_point(.5,.5));
    o2pt.push_back(g_point(0,.5));
    o2pt.push_back(g_point(1,.5));
    o4pt.push_back(g_point(0,.25));
    o4pt.push_back(g_point(1,.25));
    o4pt.push_back(g_point(1,.75));
    o4pt.push_back(g_point(0,.75));
   
    fig::fig_world W0; // fig для общего файла
    // поток для вывода общего файла
    std::ofstream out0((prefix+"/"+style+".fig").c_str());

    fig::fig_object box = fig::make_object("2 2 0 0 0 30 101 -1 20 0.000 0 0 7 0 0 *");
    box.push_back(Point<int>(-ll/2,0));
    box.push_back(Point<int>((ll*7)/2,0));
    box.push_back(Point<int>((ll*7)/2,ll*Z.size()));
    box.push_back(Point<int>(-ll/2,ll*Z.size()));
    box.push_back(Point<int>(-ll/2,0));
    W0.push_back(box);

    // идем по всем объектам
    for (int i = 0; i<Z.size(); i++){

      // в o - ссылка на нужный нам объект
      o = NULL;
      if (Z[i]->base_mp.Class == "POI")      o = &o1pt;
      if (Z[i]->base_mp.Class == "POLYLINE") o = &o2pt;
      if (Z[i]->base_mp.Class == "POLYGON")  o = &o4pt;
      if (o==NULL) continue;
      o->type = Z[i];

      // fig для элемента библиотеки
      fig::fig_world W;
      Z[i]->fig_add_colors(W);
      std::list<fig::fig_object> l = Z[i]->map2fig(*o, cnv);
      W.insert(W.end(), l.begin(), l.end());
      // поток для вывода элемента библиотеки
      std::ostringstream fname; 
      fname << prefix << "/" << style << "/" << i << ".fig"; 
      std::ofstream out(fname.str().c_str());
      fig::write(out, W);

      // кусочки для общего списка
      Z[i]->fig_add_colors(W0);
      std::list<fig::fig_object> l1 = Z[i]->map2fig ((*o)+ g_point(0,i), cnv);
      std::list<fig::fig_object> l2 = Z[i]->map2pfig((*o)+ g_point(2,i), cnv);
      W0.insert(W0.end(), l1.begin(), l1.end());
      W0.insert(W0.end(), l2.begin(), l2.end());
      fig::fig_object txt;
      txt.type = 4;
      txt.font = 18;
      txt.font_size = 6;
      txt.font_flags = 4;
      txt.text = Z[i]->name;
      txt.push_back(g_point(4*ll,(i+0.5)*ll));
      W0.push_back(txt);
      std::ostringstream htype; 
      htype << "0x" << std::setbase(16) << Z[i]->base_mp.Type << std::setbase(10);
      txt.text = htype.str();
      txt.sub_type = 2;
      txt.clear();
      txt.push_back(g_point(-ll,(i+0.5)*ll));
      W0.push_back(txt);
    }
    fig::write(out0, W0);
  }

  // список всех объектов 
  void print_mplist(std::ostream & s) const{
    for (int i = 0; i<Z.size(); i++){
      if (Z[i]->base_mp.Type == 0xFFFFFFFF) continue;
      s << Z[i]->base_mp.Class << "\t" << std::setbase(16)
        << " 0x" << Z[i]->base_mp.Type << "\t" << Z[i]->name << "\n";
    }
  }



};

#endif
