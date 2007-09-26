#ifndef LEGEND_H
#define LEGEND_H

// работа со всеми условными знаками

#include "zn.h"
#include "zn_p.h"
#include "zn_l.h"
#include "zn_a.h"

class legend{
  std::vector<zn*> Z;

  public:

  legend(std::string style = ""){
    // Сюда должны быть добавлены все знаки, которые есть!
    // Порядок м.б. важен. Знаки перебираются до первого совпадения

    Z.push_back(new zn_l_horiz(style));
    Z.push_back(new zn_l_asf(style));
    Z.push_back(new zn_l_greid(style));

    Z.push_back(new zn_a_der(style));

    Z.push_back(new zn_p_cerkov(style));

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
  void print_typ(std::ostream & s, const std::string & style) const{

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
      s << Z[i]->get_typ(style);
    }
  }

  // список всех объектов 
  void print_mplist(std::ostream & s, const std::string & style) const{
    for (int i = 0; i<Z.size(); i++){
      if (Z[i]->base_mp.Type == 0xFFFFFFFF) continue;
      s << Z[i]->base_mp.Class << "\t" << std::setbase(16)
        << " 0x" << Z[i]->base_mp.Type << "\t" << Z[i]->descr << "\n";
    }
  }

  // fig-файл со всеми объектами
  void print_fig_legend(std::ostream & s, const std::string & style) const{
    fig::fig_world W;
  }


};

#endif
